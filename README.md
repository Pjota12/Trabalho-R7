# Trabalho-R7
compilação nova:
gcc main.c TABM.h TABM.c TABMaux.h TABMaux.c Players.h Players.c THMSnome.h THMSnome.c THMSativos.h THMSativos.c THMSnacionalidade.h THMSnacionalidade.c THMSranking.h THMSranking.c THMScamp_ano.h THMScamp_ano.c THMScamp.h THMScamp.c THMStemporada.h THMStemporada.c q1.h q1.c q2.h q2.c q3.h q3.c q4.h q4.c q5.h q5.c q6.h q6.c q7.h q7.c q8.h q8.c q9.h q9.c -o TE

rodar: ./TE
rodar valgrind: valgrind --leak-check=full --show-leak-kinds=all ./TE