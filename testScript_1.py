def fun(_dividend, _divisor, _count):
    print("Dividend = ", _dividend)
    print("Divisor  = ", _divisor)
    print("Count    = ", _count)
    for i in range(0, _count):
        _dividend = _dividend - _divisor
        print(_dividend)
    print("\n\n")
       
  
# Defining main function
def main():
    divisor = 987654321987654321987654321123456789123456789123456789
    
    #1
    dividend = 8802316728825773518934583144009099213938728843696128629
    count = 8
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    
    #2
    dividend = 3414112717982022806078341568523092704819390278817255747
    count = 3
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    
    #3
    dividend = 5608802322399811312031687670333962179963719579750266492
    count = 5
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    
    #4
    dividend = 7793811926894710980082347204260416087236156065890847422
    count = 7
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    
    #5
    dividend = 7892365302880001532077655846238501744893193102897884459
    count = 7
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    #6
    dividend = 1961587665584687431016614095854300764990101234125903681
    count = 1
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    #7
    dividend = 8504445380814422014007088571973286565553333479133578267
    count = 8
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    #8
    dividend = 6032108049131874390058540029856322525656791661459239551
    count = 6
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    #9
    dividend = 9009564886597268924968544261657282698069397759698213476
    count = 9
    fun(_dividend = dividend, _divisor = divisor, _count = count)
    
    input("Press Enter to continue...")
    
    
  
# Using the special variable 
# __name__
if __name__=="__main__":
    main()