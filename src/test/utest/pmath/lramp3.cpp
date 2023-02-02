/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 февр. 2023 г.
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

#define MIN_RANK 8
#define MAX_RANK 16

namespace
{
    void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] + b[i] * (v1 + delta * i);
    }

    void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] - b[i] * (v1 + delta * i);
    }

    void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = b[i] * (v1 + delta * i) - a[i];
    }

    void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] * b[i] * (v1 + delta * i);
    }

    void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] / (b[i] * (v1 + delta * i));
    }

    void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = (b[i] * (v1 + delta * i)) / a[i];
    }
}

namespace lsp
{
    namespace generic
    {
        void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        }

        namespace avx
        {
            void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
            void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
//            void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
//            void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
//            void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
//            void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
//            void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
//            void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count);
        }
    )
}

typedef void (* lramp3_t)(float *dst, const float *a, const float *b, float v1, float v2, size_t count);

UTEST_BEGIN("dsp.pmath", lramp3)

    void call(const char *label, size_t align, lramp3_t func1, lramp3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x01);
                FloatBuffer src3(count, align, mask & 0x02);
                FloatBuffer src4(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                src1.randomize(0.001f, 1.0f);
                src2.copy(src1);
                src3.randomize(0.001f, 1.0f);
                src4.copy(src3);
                dst1.randomize(0.001f, 1.0f);
                dst2.copy(dst1);
                func1(dst1, src1, src3, 0.25f, 0.75f, count);
                func2(dst2, src2, src4, 0.25f, 0.75f, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(src3.valid(), "Source buffer 3 corrupted");
                UTEST_ASSERT_MSG(src4.valid(), "Source buffer 4 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    src3.dump("src3");
                    src4.dump("src4");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }

                // Call functions
                src1.randomize(0.001f, 1.0f);
                src2.copy(src1);
                src3.randomize(0.001f, 1.0f);
                src4.copy(src3);
                dst1.randomize(0.001f, 1.0f);
                dst2.copy(dst1);
                func1(dst1, src1, src3, 0.5f, 0.5f, count);
                func2(dst2, src2, src4, 0.5f, 0.5f, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(src3.valid(), "Source buffer 3 corrupted");
                UTEST_ASSERT_MSG(src4.valid(), "Source buffer 4 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    src3.dump("src3");
                    src4.dump("src4");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) call(#func, align, generic, func)

        TEST_EXPORT(lramp_add3);
        TEST_EXPORT(lramp_sub3);
        TEST_EXPORT(lramp_rsub3);
        TEST_EXPORT(lramp_mul3);
        TEST_EXPORT(lramp_div3);
        TEST_EXPORT(lramp_rdiv3);

        CALL(lramp_add3, generic::lramp_add3, 16);
        CALL(lramp_sub3, generic::lramp_sub3, 16);
        CALL(lramp_rsub3, generic::lramp_rsub3, 16);
        CALL(lramp_mul3, generic::lramp_mul3, 16);
        CALL(lramp_div3, generic::lramp_div3, 16);
        CALL(lramp_rdiv3, generic::lramp_rdiv3, 16);

        IF_ARCH_X86(CALL(lramp_add3, sse::lramp_add3, 16));
        IF_ARCH_X86(CALL(lramp_sub3, sse::lramp_sub3, 16));
        IF_ARCH_X86(CALL(lramp_rsub3, sse::lramp_rsub3, 16));
        IF_ARCH_X86(CALL(lramp_mul3, sse::lramp_mul3, 16));
        IF_ARCH_X86(CALL(lramp_div3, sse::lramp_div3, 16));
        IF_ARCH_X86(CALL(lramp_rdiv3, sse::lramp_rdiv3, 16));

        IF_ARCH_X86(CALL(lramp_add3, avx::lramp_add3, 16));
        IF_ARCH_X86(CALL(lramp_sub3, avx::lramp_sub3, 16));
        IF_ARCH_X86(CALL(lramp_rsub3, avx::lramp_rsub3, 16));
        IF_ARCH_X86(CALL(lramp_mul3, avx::lramp_mul3, 16));
        IF_ARCH_X86(CALL(lramp_div3, avx::lramp_div3, 16));
        IF_ARCH_X86(CALL(lramp_rdiv3, avx::lramp_rdiv3, 16));

        IF_ARCH_ARM(CALL(lramp_add3, neon_d32::lramp_add3, 16));
        IF_ARCH_ARM(CALL(lramp_sub3, neon_d32::lramp_sub3, 16));
        IF_ARCH_ARM(CALL(lramp_rsub3, neon_d32::lramp_rsub3, 16));
        IF_ARCH_ARM(CALL(lramp_mul3, neon_d32::lramp_mul3, 16));
        IF_ARCH_ARM(CALL(lramp_div3, neon_d32::lramp_div3, 16));
        IF_ARCH_ARM(CALL(lramp_rdiv3, neon_d32::lramp_rdiv3, 16));

//        IF_ARCH_AARCH64(CALL(lramp_add3, asimd::lramp_add3, 16));
//        IF_ARCH_AARCH64(CALL(lramp_sub3, asimd::lramp_sub3, 16));
//        IF_ARCH_AARCH64(CALL(lramp_rsub3, asimd::lramp_rsub3, 16));
//        IF_ARCH_AARCH64(CALL(lramp_mul3, asimd::lramp_mul3, 16));
//        IF_ARCH_AARCH64(CALL(lramp_div3, asimd::lramp_div3, 16));
//        IF_ARCH_AARCH64(CALL(lramp_rdiv3, asimd::lramp_rdiv3, 16));
    }
UTEST_END



