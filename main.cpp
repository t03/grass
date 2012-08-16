/* @TODO

   @DONE
    risolvere problema relativo alla commessa (es. VALD*) che in generale non è
    corretto ed in alcuni casi non viene riportato ed al suo posto si trova "

    risolvere problema dello sfrido che in alcuni casi non funziona
*/

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>

#define MAXLENGTH 200

using namespace std;

int main(int argc, char **argv)
{
    ifstream in;
    //ofstream out;

    short righeGiorno = -1, numGiorni = 0, indMaxOreGiorno = -1;
    char linea[MAXLENGTH], codice[20];
    char *pch;
    bool ultimaRiga = false;
    struct riga
    {
        int  numc;
        char col[20][20];
    };
    riga righe[20];
    float oreRiga, maxOreGiorno = 0, percentuali[20], totPercentuali;
    char oreResidue[6];
    float totPerGiorno = 0, sommaTotali = 0, percTotGiorno = 100.0;
    string stringa;
    size_t pos;
    short riproporzionare;
    unsigned short numTotRighe = -1;

    if (argc == 1)
#if SO == WINDOWS_OS
        in.open("C:\\ras.txt");
#else
        in.open("/home/dande/ras.txt");
#endif
    else
        in.open(argv[1]);

    if ( !in )
    {
        cerr << "* Errore apertura file (" << argv[1] << ")" << endl;
        return 1;
    }

    //of.open("C:\\grass.txt");

    /* leggo righe */
    while (in.getline(linea, MAXLENGTH) || !ultimaRiga)
    {
        numTotRighe++;
        /* se cambio data, devo scaricare i dati precedentemente caricati
           altrimenti continuo a caricare */

        if (in.gcount() == 0)
            ultimaRiga = true;

        /* cambio data */
        if ( (linea[0] != '\t' && righeGiorno > -1 ) || ultimaRiga )
        {
            /* scarico quando cambio data o ultima riga */
            /* ricavo percentule per riproporzionamento (dalla prima riga del giorno) */
            if (strchr(righe[0].col[righe[0].numc], 'h') != NULL)
            {
                strncpy(oreResidue, righe[0].col[righe[0].numc], strlen(righe[0].col[righe[0].numc]) - 1);
                /* val : x = 8 : 100 => x =  (val * 100) / 8 */
                percTotGiorno = ( atof(oreResidue) * 100 ) / 8;
            }
            else
            {
                percTotGiorno = 100.0;
                //strcpy(oreResidue, "8.0");
            }

            totPercentuali = 0;
            /* calcolo le percentuali considerando il totale ottenuto e la percentuale da ottenere */
            for (int i = 0; i <= righeGiorno; i++)
            {


                if (i != 0 || percTotGiorno == 100.0)
                    riproporzionare = 0;
                else
                    riproporzionare = 1;

                /* val : p_tot_g = x : 100 => x = (val * 100) / p_tot_g */
                //cout << '\t' << ( atof(righe[i].col[righe[i].numc - 1 - riproporzionare]) * percTotGiorno ) / totPerGiorno << '%' << endl;
                //printf("\t%.2f\n", ( atof(righe[i].col[righe[i].numc - 1 - riproporzionare]) * percTotGiorno ) / totPerGiorno);
                totPercentuali += percentuali[i] =  (float)((int)((( atof(righe[i].col[righe[i].numc - 1 - riproporzionare]) * percTotGiorno ) / totPerGiorno) * 100))/100.0;
                /*if (numGiorni == 2)
                    cout << "*** percentuali[" << i << "]: " << percentuali[i] << " totPercentuali: " << totPercentuali << endl;*/
            }

            percentuali[indMaxOreGiorno] += round((percTotGiorno - totPercentuali) * 100.0) / 100;
            /*
            cout << "percTotGiorno - totPercentuali = " << percTotGiorno - totPercentuali
                 << " sfrido: " << (float)((int)((percTotGiorno - totPercentuali) * 100)) / 100.0
                 << " r: " << round((percTotGiorno - totPercentuali) * 100.0) / 100 << endl;
            */

            for (int i = 0; i <= righeGiorno; i++)
            {
                /* stampo data, commessa e sottocommessa */
                if (i == 0)
                {
                    strcpy(codice, righe[0].col[1]);
                    cout << righe[0].col[0] << '\t' << righe[0].col[1] << '\t' << righe[0].col[2];
                }
                else
                    if (righe[i].col[0][0] == 'V')
                    {
                        strcpy(codice, righe[i].col[0]);
                        cout << "          \t" << codice << '\t' << righe[i].col[1];
                    }
                    else
                        cout << "          \t" << codice << '\t' << righe[i].col[0];
                if (i == righeGiorno)
                    cout << '\t' << percentuali[i] <<"\t(Tot: " << totPercentuali + round((percTotGiorno - totPercentuali) * 100.0) / 100 << ')' << endl;
                else
                    cout << '\t' << percentuali[i] << endl;

            }

            sommaTotali += totPerGiorno;
            numGiorni++;
            totPerGiorno = 0.0;
            righeGiorno = 0;
            strcpy(oreResidue, "    ");
            percTotGiorno = 100.0;
            maxOreGiorno = 0;
            indMaxOreGiorno = -1;
        }
        else
        {
            righeGiorno++;
        }

        /* carico dati della nuova riga */
        righe[righeGiorno].numc = -1;

        if (righeGiorno == 0 && linea[strlen(linea) - 1] == 'h')
            riproporzionare = 1;
        else
            riproporzionare = 0;

        pch = strtok(linea, " \t");
        while (pch != NULL)
        {
            righe[righeGiorno].numc++;
            strcpy(righe[righeGiorno].col[righe[righeGiorno].numc], pch);
            pch = strtok(NULL, " \t");
        }

        /* seleziono la colonna con le ore per la riga ed eventualmente trasformo la virgola in punto
           per eseguire i calcoli correttamente sia del totale che delle proporzioni (in fase di scaricamento) */
        stringa = string(righe[righeGiorno].col[righe[righeGiorno].numc - 1 - riproporzionare]);
        pos = stringa.find(",");
        if (pos != string::npos)
        {
            stringa.replace(pos, 1, ".");
        }
        oreRiga = atof(stringa.c_str());
        totPerGiorno += oreRiga;
        strcpy(righe[righeGiorno].col[righe[righeGiorno].numc - 1 - riproporzionare], stringa.c_str());

        /* memorizzo il massimo e l'indice per attribuire lo sfrido */
        if (oreRiga > maxOreGiorno)
        {
            maxOreGiorno = oreRiga;
            indMaxOreGiorno = righeGiorno;
        }
    }

    cout << endl << "* Totale: " << numGiorni << " giorni" << ", "
         << sommaTotali <<" ore" << ", " << numTotRighe << " righe" << endl;

    //of << endl;

    in.close();

    return 0;
}
