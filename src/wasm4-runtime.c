#include "wasm4-runtime.h"
#include "pico/m3.h"
#include "pico/m3/env.h"
#include "wasm4.h"
#include "wasm4-framebuffer.h"

static IM3Environment env = NULL;
static IM3Runtime runtime = NULL;
static IM3Module module = NULL;
static IM3Function start_fn = NULL;
static IM3Function update_fn = NULL;

static void w4_runtime_blit_sub(uint8_t *memory, const uint8_t *sprite, uint32_t x, uint32_t y, uint32_t width,
                                uint32_t height, uint32_t srcX, uint32_t srcY, uint32_t stride, uint32_t flags)
{
    bool bpp2 = (flags & 1);
    bool flipX = (flags & 2);
    bool flipY = (flags & 4);
    bool rotate = (flags & 8);
    uint32_t bpp = (uint32_t)bpp2 + 1;
    uint32_t nbits = width * height * bpp;

    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_blit(framebuffer, draw_colors, sprite, x, y, width, height, srcX, srcY, stride, bpp2,
                        flipX, flipY, rotate);
}

static void w4_runtime_blit(uint8_t *memory, const uint8_t *sprite, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                            uint32_t flags)
{
    w4_runtime_blit_sub(memory, sprite, x, y, width, height, 0, 0, width, flags);
}

static void w4_runtime_text(uint8_t *memory, const uint8_t *str, uint32_t x, uint32_t y)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_text(framebuffer, draw_colors, str, x, y);
}

static void w4_runtime_text_utf8(uint8_t *memory, const uint8_t *str, uint32_t byteLength, uint32_t x, uint32_t y)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_text_utf8(framebuffer, draw_colors, str, byteLength, x, y);
}

static void w4_runtime_text_utf16(uint8_t *memory, const uint16_t *str, uint32_t byteLength, uint32_t x, uint32_t y)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_text_utf16(framebuffer, draw_colors, str, byteLength, x, y);
}

static void w4_runtime_line(uint8_t *memory, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_line(framebuffer, draw_colors, x1, y1, x2, y2);
}

static void w4_runtime_hline(uint8_t *memory, uint32_t x, uint32_t y, uint32_t len)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_hline(framebuffer, draw_colors, x, y, len);
}

static void w4_runtime_vline(uint8_t *memory, uint32_t x, uint32_t y, uint32_t len)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_vline(framebuffer, draw_colors, x, y, len);
}

static void w4_runtime_rect(uint8_t *memory, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_rect(framebuffer, draw_colors, x, y, width, height);
}

static void w4_runtime_oval(uint8_t *memory, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint8_t *framebuffer = memory + W4_FRAMEBUFFER_OFFSET;
    uint8_t *draw_colors = memory + W4_DRAW_COLORS_OFFSET;

    w4_framebuffer_oval(framebuffer, draw_colors, x, y, width, height);
}

static m3ApiRawFunction(blit)
{
    m3ApiGetArgMem(const uint8_t *, sprite);
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, width);
    m3ApiGetArg(uint32_t, height);
    m3ApiGetArg(uint32_t, flags);

    w4_runtime_blit((uint8_t *)m3ApiOffsetToPtr(0), sprite, x, y, width, height, flags);

    m3ApiSuccess();
}

static m3ApiRawFunction(blitSub)
{
    m3ApiGetArgMem(const uint8_t *, sprite);
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, width);
    m3ApiGetArg(uint32_t, height);
    m3ApiGetArg(uint32_t, srcX);
    m3ApiGetArg(uint32_t, srcY);
    m3ApiGetArg(uint32_t, stride);
    m3ApiGetArg(uint32_t, flags);

    w4_runtime_blit_sub((uint8_t *)m3ApiOffsetToPtr(0), sprite, x, y, width, height, srcX, srcY, stride, flags);

    m3ApiSuccess();
}

static m3ApiRawFunction(line)
{
    m3ApiGetArg(uint32_t, x1);
    m3ApiGetArg(uint32_t, y1);
    m3ApiGetArg(uint32_t, x2);
    m3ApiGetArg(uint32_t, y2);

    w4_runtime_line((uint8_t *)m3ApiOffsetToPtr(0), x1, y1, x2, y2);

    m3ApiSuccess();
}

static m3ApiRawFunction(hline)
{
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, len);

    w4_runtime_hline((uint8_t *)m3ApiOffsetToPtr(0), x, y, len);

    m3ApiSuccess();
}

static m3ApiRawFunction(vline)
{
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, len);

    w4_runtime_vline((uint8_t *)m3ApiOffsetToPtr(0), x, y, len);

    m3ApiSuccess();
}

static m3ApiRawFunction(oval)
{
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, width);
    m3ApiGetArg(uint32_t, height);

    w4_runtime_oval((uint8_t *)m3ApiOffsetToPtr(0), x, y, width, height);

    m3ApiSuccess();
}

static m3ApiRawFunction(rect)
{
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);
    m3ApiGetArg(uint32_t, width);
    m3ApiGetArg(uint32_t, height);

    w4_runtime_rect((uint8_t *)m3ApiOffsetToPtr(0), x, y, width, height);

    m3ApiSuccess();
}

static m3ApiRawFunction(text)
{
    m3ApiGetArgMem(const uint8_t *, str);
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);

    w4_runtime_text((uint8_t *)m3ApiOffsetToPtr(0), str, x, y);

    m3ApiSuccess();
}

static m3ApiRawFunction(textUtf8)
{
    m3ApiGetArgMem(const uint8_t *, str);
    m3ApiGetArg(uint32_t, byteLength);
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);

    w4_runtime_text_utf8((uint8_t *)m3ApiOffsetToPtr(0), str, byteLength, x, y);

    m3ApiSuccess();
}

static m3ApiRawFunction(textUtf16)
{
    m3ApiGetArgMem(const uint16_t *, str);
    m3ApiGetArg(uint32_t, byteLength);
    m3ApiGetArg(uint32_t, x);
    m3ApiGetArg(uint32_t, y);

    w4_runtime_text_utf16((uint8_t *)m3ApiOffsetToPtr(0), str, byteLength, x, y);

    m3ApiSuccess();
}

static m3ApiRawFunction(tone)
{
    m3ApiGetArg(int, frequency);
    m3ApiGetArg(int, duration);
    m3ApiGetArg(int, volume);
    m3ApiGetArg(int, flags);

    // w4_runtimeTone(frequency, duration, volume, flags);

    m3ApiSuccess();
}

static m3ApiRawFunction(diskr)
{
    m3ApiReturnType(int);
    m3ApiGetArgMem(uint8_t *, dest);
    m3ApiGetArg(int, size);

    // m3ApiReturn(w4_runtimeDiskr(dest, size));

    m3ApiReturn(size);
}

static m3ApiRawFunction(diskw)
{
    m3ApiReturnType(int);
    m3ApiGetArgMem(const uint8_t *, src);
    m3ApiGetArg(int, size);

    // m3ApiReturn(w4_runtimeDiskw(src, size));

    m3ApiReturn(size);
}

static m3ApiRawFunction(trace)
{
    m3ApiGetArgMem(const char *, str);

    // w4_runtimeTrace((const uint8_t *)str);

    m3ApiSuccess();
}

static m3ApiRawFunction(traceUtf8)
{
    m3ApiGetArgMem(const uint8_t *, str);
    m3ApiGetArg(int, byteLength);

    // w4_runtimeTraceUtf8(str, byteLength);

    m3ApiSuccess();
}

static m3ApiRawFunction(traceUtf16)
{
    m3ApiGetArgMem(const uint16_t *, str);
    m3ApiGetArg(int, byteLength);

    // w4_runtimeTraceUtf16(str, byteLength);

    m3ApiSuccess();
}

static m3ApiRawFunction(tracef)
{
    m3ApiGetArgMem(const char *, str);
    m3ApiGetArgMem(const void *, stack);

    // w4_runtimeTracef((const uint8_t *)str, stack);

    m3ApiSuccess();
}

static void w4_link_api(IM3Module module)
{
    const char *const mName = "env";

    m3_LinkRawFunction(module, mName, "blit", "v(iiiiii)", blit);
    m3_LinkRawFunction(module, mName, "blitSub", "v(iiiiiiiii)", blitSub);
    m3_LinkRawFunction(module, mName, "line", "v(iiii)", line);
    m3_LinkRawFunction(module, mName, "hline", "v(iii)", hline);
    m3_LinkRawFunction(module, mName, "vline", "v(iii)", vline);
    m3_LinkRawFunction(module, mName, "oval", "v(iiii)", oval);
    m3_LinkRawFunction(module, mName, "rect", "v(iiii)", rect);
    m3_LinkRawFunction(module, mName, "text", "v(iii)", text);
    m3_LinkRawFunction(module, mName, "textUtf8", "v(iiii)", textUtf8);
    m3_LinkRawFunction(module, mName, "textUtf16", "v(iiii)", textUtf16);
    m3_LinkRawFunction(module, mName, "tone", "v(iiii)", tone);
    m3_LinkRawFunction(module, mName, "diskr", "i(ii)", diskr);
    m3_LinkRawFunction(module, mName, "diskw", "i(ii)", diskw);
    m3_LinkRawFunction(module, mName, "trace", "v(i)", trace);
    m3_LinkRawFunction(module, mName, "traceUtf8", "v(ii)", traceUtf8);
    m3_LinkRawFunction(module, mName, "traceUtf16", "v(ii)", traceUtf16);
    m3_LinkRawFunction(module, mName, "tracef", "v(ii)", tracef);
}

uint8_t *w4_runtime_init()
{
    env = m3_NewEnvironment();
    runtime = m3_NewRuntime(env, M3_RUNTIME_STACK_SIZE, NULL);

    runtime->memory.maxPages = 1;
    runtime->memory.pageSize = d_m3DefaultMemPageSize;

    M3Result res;

    res = ResizeMemory(runtime, 1);

    uint8_t *memory = m3_GetMemory(runtime, NULL, 0);

    uint32_t *palette = (uint32_t *)(memory + W4_PALETTE_OFFSET);
    palette[0] = (uint32_t)0xe0f8cf;
    palette[1] = (uint32_t)0x86c06c;
    palette[2] = (uint32_t)0x306850;
    palette[3] = (uint32_t)0x071821;

    uint16_t *colors = (uint16_t *)(memory + W4_DRAW_COLORS_OFFSET);
    colors[0] = 0x0312;

    return memory;
}

void w4_runtime_load_wasm(const uint8_t *const wasm_data, uint32_t wasm_data_size)
{
    M3Result res;

    res = m3_ParseModule(env, &module, wasm_data, wasm_data_size);

    module->memoryImported = true;

    res = m3_LoadModule(runtime, module);

    w4_link_api(module);

    res = m3_RunStart(module);
    res = m3_FindFunction(&update_fn, runtime, "update");
    res = m3_FindFunction(&start_fn, runtime, "start");
}

void w4_runtime_update(uint8_t *memory)
{
    if (start_fn)
    {
        m3_CallV(start_fn);
        start_fn = NULL;
    }
    else if (!(memory[W4_SYSTEM_FLAGS_OFFSET] &
               W4_SYSTEM_FLAGS_PRESERVE_FRAMEBUFFER))
    {
        memset(memory + W4_FRAMEBUFFER_OFFSET, 0, W4_FB_SIZE);
    }

    if (update_fn)
    {
        m3_CallV(update_fn);
    }
}
