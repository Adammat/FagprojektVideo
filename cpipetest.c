int main()
{
    char input[256];
    
    while(1){
         scanf("%s", input);
         printf("Response: %s ",input);
         printf("| Length = %d ",strlen(input));
         ext_test(input);
    }

    return 0;
}

void ext_test(char* input){
    
    
    printf("| ext_test output = [ %s ]\n",input);
    
}
