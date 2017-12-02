#include <stdio.h>   
#include <string.h>

int main(void)
{
 char filename[] = "/var/log/kern.log"; //文件名  
  FILE *fp;   
  char strLine[512];             //每行最大读取的字符数,可根据实际情况扩大  
 if((fp = fopen(filename,"r")) == NULL) //判断文件是否存在及可读  
 {   
     printf("error!");
 }
int start = 0;
int end = 0;
 while (!feof(fp))   
 {   
    fgets(strLine,512,fp);  //读取一行,并定位到下一行  

    int i=5;
    while (strLine[++i] != '\0')
        if (strLine[i-6] == 'G' && strLine[i-5] == 'o' &&strLine[i-4] == 'o' 
                &&strLine[i-3] == 'd' &&strLine[i-2] == 'b' &&strLine[i-1] == 'y'&&strLine[i] == 'e')
            {
                end = 1;
                break;
            }       
    i = 5;
    while (strLine[++i] != '\0')
    if (strLine[i-6] == 'T' && strLine[i-5] == 'r' &&strLine[i-4] == 'a' 
            &&strLine[i-3] == 'v' &&strLine[i-2] == 'e' &&strLine[i-1] == 'l'&&strLine[i] == '2')
        {
            start = 1;
            end = 0;
            break;
        }   
    if (start == 1)
    {
    i = 0;
    while (strLine[i] != '\0' && strLine[i] != ']')
        i++;
    i++;
    printf("%s", strLine+i); //输出
    }  
    if (start==1 && end == 1)
    break;
 }   
 fclose(fp);                     //关闭文件   
}  
