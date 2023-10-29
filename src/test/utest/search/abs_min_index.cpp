/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/test-fw/helpers.h>

namespace lsp
{
    namespace generic
    {
        size_t  abs_min_index(const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            size_t  abs_min_index(const float *src, size_t count);
        }

        namespace avx2
        {
            size_t  abs_min_index(const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            size_t  abs_min_index(const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            size_t  abs_min_index(const float *src, size_t count);
        }
    )

    typedef size_t  (* cond_index_t)(const float *src, size_t count);
}

UTEST_BEGIN("dsp.search", abs_min_index)

    void call(const char *label, size_t align, cond_index_t func1, cond_index_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 33, 34, 35, 36, 37, 64, 65, 66, 67, 68, 69, 100, 768, 999, 1024)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize_sign();

                // Call functions
                size_t a = func1(src, count);
                size_t b = func2(src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if (a != b)
                {
                    src.dump("src");
                    UTEST_FAIL_MSG("Result of function 1 (%d) differs result of function 2 (%d)", int(a), int(b));
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        IF_ARCH_X86(CALL(generic::abs_min_index, sse2::abs_min_index, 16));
        IF_ARCH_X86(CALL(generic::abs_min_index, avx2::abs_min_index, 32));
        IF_ARCH_ARM(CALL(generic::abs_min_index, neon_d32::abs_min_index, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_min_index, asimd::abs_min_index, 16));
    }
UTEST_END



