#include "pico/wasm.h"
#include "wasm_c_stack.h"
#include "wasm_locals.h"
#include "wasm_v_stack.h"
#include <pico/wasm/c_stack.h>
#include <stdint.h>
#include <string.h>

void wasm_vm_func_call(wasm_vm_t *vm, wasm_func_t *func) {
  wasm_c_frame_t *frame = wasm_c_stack_push_frame(&vm->c_stack);

  frame->return_pc = vm->pc; //?
  frame->code_end = func->code + func->size;
  frame->l_count = func->locals;
  frame->l_base =
      func->locals ? wasm_locals_alloc(&vm->locals, func->locals) : 0;

  vm->pc = func->code;
}

void wasm_vm_func_return(wasm_vm_t *vm) {
  wasm_c_frame_t *frame = wasm_c_stack_pop_frame(&vm->c_stack);

  if (frame->l_count) {
    wasm_locals_free(&vm->locals, frame->l_count);
  }

  vm->pc = frame->return_pc;
}

int32_t host_dummy_fn(void *vm) { return 0; }

host_func_entry_t host_funcs[] = {
    {"blit", host_dummy_fn}, {"rect", host_dummy_fn}, {"line", host_dummy_fn},
    {"text", host_dummy_fn}, {"tone", host_dummy_fn},
};

static uint8_t read_u8(wasm_reader_t *r) { return *r->ptr++; }

static uint32_t read_leb_u32(wasm_reader_t *r) {
  uint32_t result = 0;
  int shift = 0;

  while (1) {
    uint8_t b = *r->ptr++;

    result |= (b & 0x7f) << shift;

    if (!(b & 0x80))
      break;

    shift += 7;
  }

  return result;
}

static int wasm_check_header(wasm_reader_t *r) {
  if (read_u8(r) != 0x00)
    return 0;
  if (read_u8(r) != 0x61)
    return 0;
  if (read_u8(r) != 0x73)
    return 0;
  if (read_u8(r) != 0x6D)
    return 0;

  if (read_u8(r) != 0x01)
    return 0;
  if (read_u8(r) != 0x00)
    return 0;
  if (read_u8(r) != 0x00)
    return 0;
  if (read_u8(r) != 0x00)
    return 0;

  return 1;
}

static wasm_str_t read_string(wasm_reader_t *r) {
  wasm_str_t s;

  s.len = read_leb_u32(r);
  s.ptr = (const char *)r->ptr;

  r->ptr += s.len;

  return s;
}

static void build_jump_table(wasm_func_t *f) {
  uint8_t *pc = f->code;
  uint8_t *end = f->code + f->size;

  uint16_t stack[32];
  int sp = 0;

  while (pc < end) {
    uint8_t op = *pc;

    uint16_t pos = pc - f->code;

    switch (op) {

    case 0x02: // block
    case 0x03: // loop
    case 0x04: // if
      stack[sp++] = pos;
      pc++;
      pc++; // blocktype
      break;

    case 0x05: // else
    {
      uint16_t start = stack[sp - 1];

      f->jump[start] = pos;

      pc++;
    } break;

    case 0x0B: // end
    {
      uint16_t start = stack[--sp];

      f->jump[start] = pos;

      pc++;
    } break;

    default:
      pc++;
    }
  }
}

static void parse_type_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    read_u8(r); // func type (0x60)

    uint32_t params = read_leb_u32(r);

    for (uint32_t p = 0; p < params; p++)
      read_u8(r);

    uint32_t results = read_leb_u32(r);

    for (uint32_t p = 0; p < results; p++)
      read_u8(r);

    m->types[m->type_count].param_count = params;
    m->types[m->type_count].result_count = results;

    m->type_count++;
  }
}

static void parse_function_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    m->func_types[m->func_count] = read_leb_u32(r);
    m->func_count++;
  }
}

static void parse_code_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t body_size = read_leb_u32(r);

    uint8_t *body_start = r->ptr;
    uint8_t *body_end = r->ptr + body_size;

    uint32_t local_count = read_leb_u32(r);

    uint32_t locals = 0;

    for (uint32_t l = 0; l < local_count; l++) {
      uint32_t n = read_leb_u32(r);
      read_u8(r);
      locals += n;
    }

    m->funcs[i].code = r->ptr;
    m->funcs[i].size = body_end - r->ptr;
    m->funcs[i].locals = locals;
    m->funcs[i].params = m->types[m->func_types[i]].param_count;

    r->ptr = body_end;
  }
}

static void parse_import_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    wasm_str_t module = read_string(r);
    wasm_str_t name = read_string(r);

    uint8_t kind = read_u8(r);

    if (kind == 0) // function
    {
      uint32_t type = read_leb_u32(r);

      wasm_import_func_t *imp = &m->imports[m->import_count++];

      imp->module = module.ptr;
      imp->name = name.ptr;
      imp->type_index = type;
    } else {
      // skip other kinds
    }
  }
}

static void parse_export_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    wasm_str_t name = read_string(r);

    uint8_t kind = read_u8(r);
    uint32_t index = read_leb_u32(r);

    wasm_export_t *e = &m->exports[m->export_count++];

    e->name = name.ptr;
    e->kind = kind;
    e->index = index;
  }
}

static void parse_table_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    uint8_t elemtype = read_u8(r); // 0x70 = funcref

    uint8_t flags = read_u8(r);

    uint32_t min = read_leb_u32(r);

    uint32_t max = min;

    if (flags & 1)
      max = read_leb_u32(r);

    m->table.size = min;
  }
}

static void parse_element_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t flags = read_leb_u32(r);

    if (flags != 0)
      return; // пока игнорируем другие типы

    uint8_t opcode = read_u8(r); // i32.const
    uint32_t offset = read_leb_u32(r);
    read_u8(r); // end

    uint32_t func_count = read_leb_u32(r);

    for (uint32_t j = 0; j < func_count; j++) {
      uint32_t f = read_leb_u32(r);

      m->table.entries[offset + j] = f;
    }
  }
}

static void parse_data_section(wasm_module_t *m, wasm_reader_t *r) {
  uint32_t count = read_leb_u32(r);

  for (uint32_t i = 0; i < count; i++) {
    uint32_t flags = read_leb_u32(r);

    if (flags != 0)
      return; // пока игнорируем другие типы

    uint8_t op = read_u8(r); // i32.const

    uint32_t offset = read_leb_u32(r);

    read_u8(r); // end

    uint32_t size = read_leb_u32(r);

    memcpy(m->memory + offset, r->ptr, size);

    r->ptr += size;
  }
}

int wasm_load(wasm_module_t *m, uint8_t *wasm, uint32_t size) {
  wasm_reader_t r;

  r.ptr = wasm;
  r.end = wasm + size;

  if (!wasm_check_header(&r))
    return -1;

  while (r.ptr < r.end) {
    uint8_t id = read_u8(&r);
    uint32_t section_size = read_leb_u32(&r);

    uint8_t *section_end = r.ptr + section_size;

    switch (id) {
    case 1:
      parse_type_section(m, &r);
      break;
    case 2:
      parse_import_section(m, &r);
      break;
    case 3:
      parse_function_section(m, &r);
      break;
    case 4:
      parse_table_section(m, &r);
      break;
    case 7:
      parse_export_section(m, &r);
      break;
    case 9:
      parse_element_section(m, &r);
      break;
    case 10:
      parse_code_section(m, &r);
      break;
    case 11:
      parse_data_section(m, &r);
      break;

    default:
      r.ptr = section_end;
    }

    r.ptr = section_end;
  }

  return 0;
}

static void bind_import(wasm_module_t *m, uint32_t import_index,
                        host_func_t fn) {
  m->func_table[import_index].kind = 1;
  m->func_table[import_index].host = fn;
}

void wasm_resolve_imports(wasm_module_t *m) {
  for (int i = 0; i < m->import_count; i++) {
    const char *name = m->imports[i].name;

    for (int j = 0; j < 5; j++) {
      if (!strcmp(name, host_funcs[j].name)) {
        bind_import(m, i, host_funcs[j].fn);
        break;
      }
    }
  }
}

int wasm_find_export(wasm_module_t *m, const char *name) {
  for (int i = 0; i < m->export_count; i++) {
    if (!strcmp(m->exports[i].name, name))
      return m->exports[i].index;
  }

  return -1;
}

void wasm_finalize_functions(wasm_module_t *m) {
  for (int i = 0; i < m->func_count; i++) {
    int index = m->import_count + i;

    m->func_table[index].kind = 0;
    m->func_table[index].wasm = &m->funcs[i];
  }

  m->total_func_count = m->import_count + m->func_count;
}

static void exec_loop(wasm_vm_t *vm) {
  wasm_c_stack_t *cs = &vm->c_stack;
  wasm_v_stack_t *vs = &vm->v_stack;

  while (!wasm_c_stack_is_empty(cs)) {
    wasm_c_frame_t *f = wasm_c_stack_peek_frame(cs);

    uint8_t opcode = *vm->pc++;

    switch (opcode) {
    case 0x41: // i32.const
    {
      int32_t v = read_leb_u32(&vm->pc);
      wasm_v_stack_push_i32(vs, v);
    } break;
    case 0x20: // local.get
    {
      uint32_t idx = read_leb_u32(&vm->pc);
      vs->data[vs->sp++] = f->locals[idx];
    } break;

    case 0x21: // local.set
    {
      uint32_t idx = read_leb_u32(&f->pc);
      f->locals[idx] = vs->data[--vs->sp];
    } break;
    case 0x6A: // i32.add
    {
      int32_t b = vs->data[--vs->sp].i32;
      int32_t a = vs->data[--vs->sp].i32;

      vs->data[vs->sp++].i32 = a + b;
    } break;
    case 0x28: // i32.load
    {
      read_leb_u32(&f->pc); // align
      uint32_t offset = read_leb_u32(&f->pc);

      uint32_t addr = vs->data[--vs->sp].u32 + offset;

      int32_t val;

      memcpy(&val, vm->module.memory + addr, 4);

      vs->data[vs->sp++].i32 = val;
    } break;
    case 0x10: {
      uint32_t idx = read_leb_u32(&f->pc);

      wasm_func_ref_t *fn = &vm->module.func_table[idx];

      if (fn->kind) {
        fn->host(vm);
      } else {
        push_frame(vm, fn->wasm);
      }
    } break;
    case 0x0F: {
      pop_frame(vm);
    } break;
    case 0x0B: {
      vm->csp--;
      if (f->pc >= f->code_end)
        pop_frame(vm);
    } break;
    case 0x02: {
      uint16_t pos = f->pc - f->code;

      uint16_t end = f->jump[pos];

      vm->control_stack[vm->csp++] =
          (control_frame_t){.target = f->code + end, .type = 0};

      f->pc++; // blocktype
    } break;
    case 0x03: {
      uint16_t pos = f->pc - f->code;

      vm->control_stack[vm->csp++] =
          (control_frame_t){.target = f->pc, .type = 1};

      f->pc++; // blocktype
    } break;
    case 0x04: {
      int cond = vs->data[--vs->sp].i32;

      uint16_t pos = f->pc - f->code;

      uint16_t jump = f->jump[pos];

      if (!cond)
        f->pc = f->code + jump;

      vm->control_stack[vm->csp++] =
          (control_frame_t){.target = f->code + jump};

    } break;
    case 0x0C: {
      uint32_t depth = read_leb_u32(&f->pc);

      control_frame_t *cf = &vm->control_stack[vm->csp - 1 - depth];

      f->pc = cf->target;

      vm->csp -= depth + 1;
    } break;
    case 0x0D: {
      uint32_t depth = read_leb_u32(&f->pc);

      int cond = vs->data[--vs->sp].i32;

      if (cond) {
        control_frame_t *cf = &vm->control_stack[vm->csp - 1 - depth];

        f->pc = cf->target;

        vm->csp -= depth + 1;
      }
    } break;
    }
  }
}

void wasm_call(wasm_vm_t *vm, uint32_t func_index) {
  wasm_func_ref_t *f = &vm->module.func_table[func_index];

  push_frame(vm, f->wasm);

  exec_loop(vm);
}
