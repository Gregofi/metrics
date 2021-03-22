

#define hideif(x) if(x)


int main(void)
{
    hideif(1 < 2)
        return 1;
    hideif(1 < 2)
        hideif(2 < 3)
            return 2;

}