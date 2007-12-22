#ifndef UNDOREDO_H
#define UNDOREDO_H

namespace breathe
{
  class cUndoRedoState
  {
  public:
  cUndoRedoState(const std::string& title);

  const std::string& GetTitle() const;

  void Undo();
  void Redo();

  private:
  virtual void _Undo() = 0; 
  virtual void _Redo() = 0;

  const std::string _title;

  NO_COPY();
  };

  cUndoRedoState::cUndoRedoState(const std::string& title) :
  _title(title)
  {
  }

  void cUndoRedoState::Undo() 
  {
  _Undo();
  }

  void cUndoRedoState::Redo()
  {
  _Redo();
  }


  // Don't call delete on any state that is passed into this class.  
  // This class takes care of it for you so you don't have to keep your own list.  
  class cUndoRedo
  {
  public:
  ~cUndoRedo();

  void push_back(cUndoRedoState* state);

  void Undo();
  void Redo();

  bool CanUndo() const { return !undo.empty(); }
  bool CanRedo() const { return !redo.empty(); } 

  const std::string& GetUndoTitle() const { assert(CanUndo()); return undo.back()->GetTitle(); }
  const std::string& GetRedoTitle() const { assert(CanRedo()); return redo.back()->GetTitle(); } 

  private:
  std::list<cUndoRedoState*> undo;
  std::list<cUndoRedoState*> redo;

  void ClearRedo();

  NO_COPY();
  };

  cUndoRedo::~cUndoRedo()
  {
  while (!undo.empty()) { 
  SAFE_DELETE(undo.back());
  undo.pop_back();
  };

  ClearRedo();
  }

  void cUndoRedo::ClearRedo()
  {   
  while (!redo.empty()) {
  SAFE_DELETE(redo.back());
  redo.pop_back();
  };
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

#endif //UNDOREDO_H
