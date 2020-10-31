#include <new>

template <typename T>
class ChunkAllocator;

template <typename T>
class Chunk {
public:
    friend ChunkAllocator<T>;

    Chunk(size_t n, Chunk* prev = nullptr) {
        this->data = (T*)(new uint8_t(n * sizeof(T)));
        this->free = this->data;
        this->prev = prev;
        free_size = n;
    }

    bool can_allocate(size_t n) const {
        return n <= free_size;
    }

    T* allocate(size_t n) {
        T* result = free;

        free += n * sizeof(T);
        free_size -= n;

        return result;
    }

    ~Chunk() {
        delete[] (uint8_t*) data;
    }
private:
    T* data;
    size_t free_size;
    T* free;
    Chunk* prev;
};

template <typename T>
class ChunkAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    template <class U> struct rebind { typedef ChunkAllocator<U> other; };

    ChunkAllocator(size_t chunk_n = 1024u) : chunk_n(chunk_n) {
        last = new Chunk<T>(chunk_n);
        copy_counter = new size_t(1);
    }

    ChunkAllocator(const ChunkAllocator& other) : chunk_n(other.chunk_n) {
        this->last = other.last;
        this->copy_counter = other.copy_counter;

        ++(*this->copy_counter);
    }

    ChunkAllocator& operator=(const ChunkAllocator& other) {
        if (this == &other) {
            return *this;
        }

        if (*copy_counter == 1) {
            while (last) {
                Chunk<T>* chunk = last;
                last = last->prev;
                delete chunk;
            }
            delete copy_counter;
        } else {
            copy_counter--;
        }

        *this = ChunkAllocator(other);
        return *this;
    }

    T* allocate(const size_t n) {
        if (n > chunk_n) {
            throw std::bad_alloc();
        }

        Chunk<T>* chunk = last;

        while (chunk) {
            if (chunk->can_allocate(n)) {
                return chunk->allocate(n);
            } else {
                chunk = chunk->prev;
            }
        }

        Chunk<T> new_chunk = new Chunk(chunk_n, last);
        this->last = new_chunk;

        return this->last->allocate(chunk);
    }

    void deallocate(T* ptr, const size_t n) {}

    template <typename ... Args>
    void construct(T* p, const Args... args) {
        return new (p) T(args...);
    }

    void destroy(T* p) {
        p->~T();
    }

    ~ChunkAllocator() {
        if (*copy_counter == 1) {
            while (last) {
                Chunk<T>* chunk = last;
                last = last->prev;
                delete chunk;
            }
            delete copy_counter;
        } else {
            copy_counter--;
        }
    }

private:
    const size_t chunk_n;
    Chunk<T>* last = nullptr;
    size_t* copy_counter;
};
