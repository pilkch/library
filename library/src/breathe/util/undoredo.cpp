#include <cassert>

#include <vector>
#include <list>

#include <string>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/undoredo.h>

namespace breathe
{
  cUndoRedo::~cUndoRedo()
  {
    while (!undo.empty()) { 
      SAFE_DELETE(undo.back());
      undo.pop_back();
    }

    ClearRedo();
  }

  void cUndoRedo::ClearRedo()
  {   
    while (!redo.empty()) {
      SAFE_DELETE(redo.back());
      redo.pop_back();
    }
  }

  void cUndoRedo::push_back(cUndoRedoState* state)
  {
    undo.push_back(state);

    ClearRedo();
  }

  void cUndoRedo::Undo()
  {
    assert(CanUndo());

    cUndoRedoState* state = undo.back();
    undo.pop_back();

    state->Undo();

    redo.push_back(state);
  }

  void cUndoRedo::Redo()
  {
    assert(CanRedo());

    cUndoRedoState* state = redo.back();
    redo.pop_back();

    state->Redo();

    undo.push_back(state);
  }
}
