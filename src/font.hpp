/*******************************************************
* Copyright (c) 2015-2019, ArrayFire
* All rights reserved.
*
* This file is distributed under 3-clause BSD license.
* The complete license agreement can be obtained at:
* http://arrayfire.com/licenses/BSD-3-Clause
********************************************************/

#pragma once

#include <common.hpp>
#include <vector>
#include <memory>
#include <map>

static const int NUM_CHARS = 95;

namespace internal
{

class font_impl {
    private:
        /* VAO map to store a vertex array object
         * for each valid window context */
        std::map<int, GLuint> mVAOMap;

        /* attributes */
        bool mIsFontLoaded;
        std::string mTTFfile;
        std::vector<float> mVertexData;
        int mWidth;
        int mHeight;
        GLuint mVBO;
        GLuint mProgram;
        GLuint mSampler;

        GLuint mCharTextures[NUM_CHARS];
        int mAdvX[NUM_CHARS], mAdvY[NUM_CHARS];
        int mBearingX[NUM_CHARS], mBearingY[NUM_CHARS];
        int mCharWidth[NUM_CHARS], mCharHeight[NUM_CHARS];
        int mLoadedPixelSize, mNewLine;

        /* helper function to extract glyph of
         * ASCII character pointed by pIndex*/
        void extractGlyph(int pIndex);

        /* helper functions to bind and unbind
         * rendering resources */
        void bindResources(int pWindowId);
        void unbindResources() const;

        /* helper to destroy GL objects created for
         * given font face and size if required */
        void destroyGLResources();

    public:
        font_impl();
        ~font_impl();

        void setOthro2D(int pWidth, int pHeight);
        void loadFont(const char* const pFile, int pFontSize);
        void loadSystemFont(const char* const pName, int pFontSize);

        void render(int pWindowId,
                    const float pPos[2], const float pColor[4], const char* pText,
                    int pFontSize = -1, bool pIsVertical = false);
};

class _Font {
    private:
        std::shared_ptr<font_impl> fnt;

    public:
        _Font() : fnt(std::make_shared<font_impl>()) {}

        const std::shared_ptr<font_impl>& impl() const {
            return fnt;
        }

        inline void setOthro2D(int pWidth, int pHeight) {
            fnt->setOthro2D(pWidth, pHeight);
        }

        inline void loadFont(const char* const pFile, int pFontSize) {
            fnt->loadFont(pFile, pFontSize);
        }

        inline void loadSystemFont(const char* const pName, int pFontSize) {
            fnt->loadSystemFont(pName, pFontSize);
        }
};

}
