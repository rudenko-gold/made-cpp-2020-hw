#include <new>

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

    static const size_type chunk_n = 1024u;

    ChunkAllocator() {
        head = new Chunk(chunk_n);
        copy_counter = new size_type(1);
    }

    ChunkAllocator(const ChunkAllocator& other) {
        this->head = other.head;
        this->copy_counter = other.copy_counter;

        ++(*this->copy_counter);
    }

    ChunkAllocator& operator=(const ChunkAllocator& other) {
        if (this == &other) {
            return *this;
        }

        if (*copy_counter == 1) {
            while (head) {
                Chunk* to_delete = head;
                head = head->next;
                delete to_delete;
            }

            delete copy_counter;
        } else {
            copy_counter--;
        }

        *this = ChunkAllocator(other);
        return *this;
    }

    pointer allocate(const size_type n) {
        if (n > chunk_n) {
            throw std::bad_alloc();
        }

        Chunk* chunk = head;

        while (chunk->next) {
            if (chunk->can_allocate(n)) {
                return chunk->allocate(n);
            } else {
                chunk = chunk->next;
            }
        }

        if (chunk->can_allocate(n)) {
            return chunk->allocate(n);
        } else {
            Chunk* new_chunk = new Chunk(n);
            chunk->next = new_chunk;

            return new_chunk->allocate(n);
        }
    }

    void deallocate(pointer ptr, const size_type n) {}

    template <typename ... Args>
    void construct(pointer p, const Args&&... args) {
        return new (p) T(std::forward<Args>(args)...);
    }

    void destroy(pointer p) {
        p->~T();
    }

    ~ChunkAllocator() {
        if (*copy_counter == 1) {
            while (head) {
                Chunk* to_delete = head;
                head = head->next;
                delete to_delete;
            }

            delete copy_counter;
        } else {
            copy_counter--;
        }
    }

private:
    class Chunk {
    public:
        friend ChunkAllocator<value_type>;

        Chunk(size_t n) {
            this->data = (pointer)(new uint8_t(n * sizeof(value_type)));
            this->free = this->data;
            free_size = n;
        }

        bool can_allocate(size_t n) const {
            return n <= free_size;
        }

        pointer allocate(size_type n) {
            pointer result = free;

            free += n * sizeof(value_type);
            free_size -= n;

            return result;
        }

        ~Chunk() {
            delete[] (uint8_t*) data;
        }

    private:
        pointer data;
        size_type free_size;
        pointer free;
        Chunk* next;
    };

    Chunk* head = nullptr;
    size_type* copy_counter;
};
