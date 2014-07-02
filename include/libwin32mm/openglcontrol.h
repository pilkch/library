#ifndef LIBWIN32MM_OPENGLCONTROL_H
#define LIBWIN32MM_OPENGLCONTROL_H

#undef interface

// libwin32mm headers
#include <libwin32mm/controls.h>

#undef interface
#define interface Interface

namespace win32mm
{
  typedef uint32_t key_t;

  class cKeyEvent {
  public:
    cKeyEvent();

    key_t key;
    bool bControl;
    bool bAlt;
    bool bShift;
  };

  // ** cOpenGLControl

  class cOpenGLControl {
  public:
     cOpenGLControl();
     ~cOpenGLControl();

     void Create(cWindow& parent, int idControl);

     bool IsValid() const { return (control != NULL); }

     HWND GetHandle();

     int GetWidth() const;
     int GetHeight() const;

     void Update();

  private:
     static void RegisterClass();

     void UpdateSize();

     bool IsEnabled() const;

     void Enable();
     void Disable();

     void SetCapture();  
     void ReleaseCapture();
     bool IsCaptured() const;
     bool IsKeyboardFocused() const;

     void Update(int x, int y, int width, int height);

     void Paint();

     virtual void OnMouseIn() {}
     virtual void OnMouseOut() {}
     virtual void OnMouseMove(int x, int y)     { (void)x; (void)y; }
     virtual void OnMouseHover(int x, int y)    { (void)x; (void)y; }
     virtual void OnLButtonDown(int x, int y)   { (void)x; (void)y; }
     virtual void OnLButtonUp(int x, int y)     { (void)x; (void)y; }
     virtual void OnRButtonUp(int x, int y)     { (void)x; (void)y; }
     virtual void OnRButtonDown(int x, int y)   { (void)x; (void)y; }
     virtual void OnMButtonUp(int x, int y) { (void)x; (void)y; }
     virtual void OnMButtonDown(int x, int y) { (void)x; (void)y; }
     virtual void OnMouseWheel(int x, int y, int iDeltaUp) { (void)x; (void)y; (void)iDeltaUp; }
     virtual void OnDoubleClick(int x, int y) { (void)x; (void)y; }
     virtual bool OnKeyDown(const cKeyEvent& event) { (void)event; return false; }
     virtual bool OnKeyUp(const cKeyEvent& event) { (void)event; return false; }

     virtual void OnSize() = 0;
     virtual void OnPaint() = 0;

     void EvPaint();

     virtual LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
     static LRESULT CALLBACK _WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

     HWND control;

     int iWidth;
     int iHeight;

     bool bMouseIn;
  };
}

#endif // LIBWIN32MM_OPENGLCONTROL_H
