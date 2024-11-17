#ifndef UNDOREDO_H
#define UNDOREDO_H

namespace spitfire
{
  class cUndoRedoState
  {
  public:
    explicit cUndoRedoState(const string_t& title);
    virtual ~cUndoRedoState() {}

    const string_t& GetTitle() const { return _title; }

    void Undo() { _Undo(); }
    void Redo() { _Redo(); }

  private:
    virtual void _Undo() = 0; 
    virtual void _Redo() = 0;

    const string_t _title;

    // No copy
    cUndoRedoState(const cUndoRedoState&) = delete;
    cUndoRedoState& operator=(const cUndoRedoState&) = delete;
    cUndoRedoState(cUndoRedoState&&) noexcept = delete;
    cUndoRedoState& operator=(cUndoRedoState&&) noexcept = delete;
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

    const string_t& GetUndoTitle() const { assert(CanUndo()); return undo.back()->GetTitle(); }
    const string_t& GetRedoTitle() const { assert(CanRedo()); return redo.back()->GetTitle(); }

  private:
    std::list<cUndoRedoState*> undo;
    std::list<cUndoRedoState*> redo;

    void ClearRedo();

    // No copy
    cUndoRedo(const cUndoRedo&) = delete;
    cUndoRedo& operator=(const cUndoRedo&) = delete;
    cUndoRedo(cUndoRedo&&) noexcept = delete;
    cUndoRedo& operator=(cUndoRedo&&) noexcept = delete;
  };

  //TODO: Move this to the cpp file

  inline cUndoRedoState::cUndoRedoState(const string_t& title) :
    _title(title)
  {
  }
}

#endif // UNDOREDO_H
