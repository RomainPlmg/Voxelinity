#ifndef __GRAHIC_STATE_GUARD_H__
#define __GRAHIC_STATE_GUARD_H__

#include "GraphicContext.h"

class GraphicStateGuard {
   public:
    GraphicStateGuard() = default;
    ~GraphicStateGuard() = default;

    void Save();
    void Restore();

   private:
    GLboolean m_DepthTestEnabled;
    GLboolean m_BlendEnabled;
    GLboolean m_LineSmoothEnabled;
    GLboolean m_FaceCullingEnabled;
    GLboolean m_MultiSample;

    void SetGLState(GLenum capability, GLboolean enabled) {
        if (enabled) {
            glEnable(capability);
        } else {
            glDisable(capability);
        }
    }
};

#endif  //__GRAHIC_STATE_GUARD_H__