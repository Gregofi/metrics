//
// Created by filip on 12/27/20.
//

int foo()
{
    int i;
    int c;
    double r = c + i;
    r *= 3;
    return 1;
}

int func1()
{
    int i;
    if(i < 10)
    {
        return 0;
    }
    else if(i < 20)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

int func2()
{
    while(true)
    {
        for(;;)
        {
            while(true)
            {
                for(;;)
                {
                    return 1;
                }
            }
        }

        switch(true)
        {
            case 1:
            {
                return 2;
            }
        }
    }
}

int func3()
{
    {
        {
            {
                {
                    return 0;
                }
            }
        }
    }
}