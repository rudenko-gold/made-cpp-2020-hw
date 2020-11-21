#pragma once

namespace task {

    template <class T>
    class SharedPtr;

    template <class T>
    class WeakPtr;

    template <class T>
    class UniquePtr {
    public:
        UniquePtr() {}

        explicit UniquePtr(T* ptr);

        UniquePtr(const UniquePtr& other) = delete;
        UniquePtr operator=(const UniquePtr& other) = delete;

        UniquePtr(UniquePtr&& other);

        UniquePtr& operator=(UniquePtr&& other);

        T* get() const;
        T& operator*() const;
        T* operator->() const;
        T* get();
        T& operator*();
        T* operator->();

        T* release();
        void reset(T* ptr = 0);
        void swap(UniquePtr& other);
        ~UniquePtr();
    private:
        T* data = nullptr;
    };

    template <typename T>
    class ControlBlock {
    public:
        friend SharedPtr<T>;
        friend WeakPtr<T>;

        ControlBlock(T* ptr) : ptr(ptr), ref_count(1), weak_count(0) {}

        ~ControlBlock() {
            delete ptr;
        }
    private:
        T* ptr = nullptr;
        size_t ref_count;
        size_t weak_count;
    };

    template <typename T>
    class SharedPtr {
    public:
        friend WeakPtr<T>;

        explicit SharedPtr(T* ptr = 0);
        SharedPtr(const SharedPtr& other);
        SharedPtr(SharedPtr&& other);
        SharedPtr& operator=(const SharedPtr& other);
        SharedPtr& operator=(SharedPtr&& other);
        SharedPtr(const WeakPtr<T>& other);

        T* get();
        T& operator*();
        T* operator->();

        size_t use_count() const;
        void reset(T* ptr = 0);
        void swap(SharedPtr& other);

        ~SharedPtr();

    private:
        ControlBlock<T>* control_block;
    };

    template <typename T>
    class WeakPtr {
    public:
        friend SharedPtr<T>;

        WeakPtr();
        WeakPtr(const SharedPtr<T>& other);
        WeakPtr(const WeakPtr& other);
        WeakPtr(WeakPtr&& other);
        WeakPtr& operator=(const WeakPtr& other);
        WeakPtr& operator=(WeakPtr&& other);
        WeakPtr& operator=(const SharedPtr<T>& other);

        SharedPtr<T> lock();
        size_t use_count();
        bool expired();
        void reset();
        void swap(WeakPtr& other);

        ~WeakPtr();

    private:
        ControlBlock<T>* control_block;
    };

}  // namespace task


#include "smart_pointers.tpp"
