# Smart-Pointer-Implementation-Cpp
Implemented a smart pointer similar to std::shared_ptr that only deletes its managed object when the associated reference count reaches 0.
Implemented smart pointer to avoid object slicing for non-virtual destructors
Developed SharedPtr class to allow different smart pointers in different threads to be safely assigned and unassigned to the same shared objects.
