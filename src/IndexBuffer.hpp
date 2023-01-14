#ifndef __IndexBuffer__
#define __IndexBuffer__

class IndexBuffer {
    public:
        IndexBuffer(const unsigned int* data, unsigned int cnt);
        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        inline unsigned int getCount() const { return count; }
    private:
        unsigned int rendererID;
        unsigned int count; //<<< Numner of indices
};

#endif // __IndexBuffer__
