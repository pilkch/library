#ifndef UNDOREDO_H
#define UNDOREDO_H

namespace breathe
{
  class cUndoRedoState
  {
  public:
    cUndoRedoState::cUndoRedoState(const breathe::string_t& title) :
      _title(title)
    {
    }

    const breathe::string_t& GetTitle() const { return _title; }

    void cUndoRedoState::Undo() { _Undo(); }
    void cUndoRedoState::Redo() { _Redo(); }

  private:
    virtual void _Undo() = 0; 
    virtual void _Redo() = 0;

    const breathe::string_t _title;

    NO_COPY(cUndoRedoState);
  };


  // Don't call delete on any state that is passed into this class.  
  // This class takes care of it for you so you don't have to keep your own list.  
  class cUndoRedo
  {
  public:
    cUndoRedo() {}
    ~cUndoRedo();

    void push_back(cUndoRedoState* state);

    void Undo();
    void Redo();

    bool CanUndo() const { return !undo.empty(); }
    bool CanRedo() const { return !redo.empty(); } 

    const breathe::string_t& GetUndoTitle() const { assert(CanUndo()); return undo.back()->GetTitle(); }
    const breathe::string_t& GetRedoTitle() const { assert(CanRedo()); return redo.back()->GetTitle(); } 

  private:
    std::list<cUndoRedoState*> undo;
    std::list<cUndoRedoState*> redo;

    void ClearRedo();

    NO_COPY(cUndoRedo);
  };
}

#endif //UNDOREDO_H
