#include <stdio.h>
#include <stdlib.h>

void InputName(char StudName[][8]){
     int i;
     for(i = 0; i < 8; i++){
          char* name = (char*)malloc(20 * sizeof(char));
          scanf("%s",name);
          *((char**)StudName + i) = name;
     }
     
}

void BubbleSort(char StudName[][8]){
     int i, j;
     for(i = 0; i < 7; i++){
           int min = i;
           for(j = i + 1; j < 7; j++){
                   if(*(*((char**)StudName + j)) > *(*((char**)StudName + i))){
                        min = j;                                                                
                   }    
           }  
           char* temp = *((char**)StudName + min);     
           *((char**)StudName + min) = *((char**)StudName + i);
           *((char**)StudName + i) = temp;     
     }
}

void OutputName(char StudName[][8]){
     int i;
     for(i = 0; i < 8; i++){
           printf("%s\n", *((char**)StudName + i));
     }
}

int main(int argc, char *argv[])
{
    char* names[8];
    InputName(names);
    BubbleSort(names);
    OutputName(names);
    system("PAUSE");	
    return 0;
}
