#pragma once

/**
 * @brief w25qxx write status register timeout definition
 */
#ifndef W25QXX_WRITE_STATUS_TIMEOUT_MS
#define W25QXX_WRITE_STATUS_TIMEOUT_MS (1000U) /**< max 1000ms */
#endif

/**
 * @brief w25qxx erase chip timeout definition
 */
#ifndef W25QXX_ERASE_CHIP_TIMEOUT_MS
#define W25QXX_ERASE_CHIP_TIMEOUT_MS (1000U * 1000U) /**< max 1000s */
#endif

/**
 * @brief w25qxx erase security timeout definition
 */
#ifndef W25QXX_ERASE_SECURITY_TIMEOUT_MS
#define W25QXX_ERASE_SECURITY_TIMEOUT_MS (100U) /**< max 100ms */
#endif

/**
 * @brief w25qxx program security timeout definition
 */
#ifndef W25QXX_PROGRAM_SECURITY_TIMEOUT_MS
#define W25QXX_PROGRAM_SECURITY_TIMEOUT_MS (3U) /**< max 3ms */
#endif

/**
 * @brief w25qxx page program timeout definition
 */
#ifndef W25QXX_PAGE_PROGRAM_TIMEOUT_MS
#define W25QXX_PAGE_PROGRAM_TIMEOUT_MS (3U) /**< max 3ms */
#endif

/**
 * @brief w25qxx erase 4k timeout definition
 */
#ifndef W25QXX_ERASE_4K_TIMEOUT_MS
#define W25QXX_ERASE_4K_TIMEOUT_MS (400U) /**< max 400ms */
#endif

/**
 * @brief w25qxx erase 32k timeout definition
 */
#ifndef W25QXX_ERASE_32K_TIMEOUT_MS
#define W25QXX_ERASE_32K_TIMEOUT_MS (1600U) /**< max 1600ms */
#endif

/**
 * @brief w25qxx erase 64k timeout definition
 */
#ifndef W25QXX_ERASE_64K_TIMEOUT_MS
#define W25QXX_ERASE_64K_TIMEOUT_MS (2000U) /**< max 2000ms */
#endif
