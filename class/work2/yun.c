#include <stdio.h>
int main()
{
    
    int gang=1,hong=1,lei=1,ming=1,le=1;
    for (;gang<=5;gang++)
    {
        for (;hong<=5;hong++)
        {
            if(hong==gang) continue;
            for (;lei<=5;lei++)
            {
                if(hong==lei||lei==gang) continue;
                for (;ming<=5;ming++)
                {
                    if(ming==hong||ming==hong||ming==lei) continue;
                    for (;le<=5;le++)
                    {
                        if(le==gang||le==hong||le==lei||le==ming) continue;
						if(le<lei&&ming<gang&&hong<le&&hong>gang)
                        {
                             printf("第1名: %s\n", (ming==1)?"小明":((gang==1)?"小刚":((hong==1)?"小红":((lei==1)?"李雷":"乐乐"))));
                            printf("第2名: %s\n", (ming==2)?"小明":((gang==2)?"小刚":((hong==2)?"小红":((lei==2)?"李雷":"乐乐"))));
                            printf("第3名: %s\n", (ming==3)?"小明":((gang==3)?"小刚":((hong==3)?"小红":((lei==3)?"李雷":"乐乐"))));
                            printf("第4名: %s\n", (ming==4)?"小明":((gang==4)?"小刚":((hong==4)?"小红":((lei==4)?"李雷":"乐乐"))));
                            printf("第5名: %s\n", (ming==5)?"小明":((gang==5)?"小刚":((hong==5)?"小红":((lei==5)?"李雷":"乐乐"))));
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}