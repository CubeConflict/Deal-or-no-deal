#include "main.h"

using namespace std;

namespace game
{
    const int maxBoxes = 12;
    const int boxValues[maxBoxes] = {1, 10, 50, 100, 500, 2000, 5000, 10000, 20000, 50000, 100000, 500000};

    struct box
    {
        int insideBox;
        bool opened;
    };
    box boxes[maxBoxes];

    struct playerinfo
    {
        int playerBox;
        int choosenBox;
        int bankGain;
    };
    struct playerinfo player;

    void assignBoxes() // assign unique value to each box
    {
        srand(time(NULL));
        loopi(maxBoxes)
        {
            boxes[i].opened = false;
            conoutf(C_GREEN, C_BLACK, "Please wait, we are assigning boxes... (%1.f%%)\n", ((float)100/maxBoxes)*i);
            bool uniqueValueFound = false;
            while(!uniqueValueFound)
            {
                boxes[i].insideBox = boxValues[rnd(maxBoxes)];
                uniqueValueFound = true;
                loopj(i)
                {
                    if(boxes[i].insideBox == boxes[j].insideBox)
                    {
                        uniqueValueFound = false;
                        break;
                    }
                }
            }
            clearConsole();
        }
    }

    void initGame() // (re)initialize everything for a new game
    {
        player.bankGain = 0;
        player.playerBox = 0;
        assignBoxes();
    }

    void runGame() // main loop, initializing game and continue/quit choice after each game
    {
        for(;;)
        {
            printf("You want to continue? (Y/N)\n");
            string response;

            while(response!="Y" || response!="N")
            {
                getline(cin, response);
                if(response=="Y")
                {
                    initGame();
                    break;
                }
                else if(response=="N") quit();
            }
            playGame();
        }
    }

    int openCount(bool remaining = false) // count the number of opened boxes (or the number of remaining)
    {
        int num = 0;
        loopi(maxBoxes) if(boxes[i].opened) num++;
        return remaining ? maxBoxes-num : num;
    }

    bool allOpened() // check if all boxes are opened
    {
        return openCount()>=maxBoxes-1; //-1 because of player box
    }

    void bankCall()
    {
        int bankOffer = 0;

        sound::playSound("bank_offer");

        loopi(maxBoxes) if(!boxes[i].opened) bankOffer+=boxes[i].insideBox;
        if(openCount(true)) bankOffer/=openCount(true)*1.3f;

        if(player.bankGain)
        {
            clearConsole();
            printf("At this time, the bank would make another offer: %d$, %s", bankOffer, player.bankGain>=bankOffer ? "well done!\n\n" : "bad luck!\n\n");
        }
        else
        {
            printf("The bank has an offer for you!\n");
            string response;
            printf("Offer: %d$. Accept that offer? (Y/N)\n", bankOffer);

            while(response!="Y" || response!="N")
            {
                getline(cin, response);
                if(response=="Y")
                {
                    player.bankGain = bankOffer;
                    sound::playSound("victory");
                    clearConsole();
                    printf("You selled your box for %d$\n\n", player.bankGain);
                    break;
                }
                else if(response=="N")
                {
                    clearConsole();
                    printf("You declined bank's offer.\n\n");
                    break;
                }
            }
        }
    }

    void drawGame() // drawing boxes numbers and content of opened boxes
    {
        loopi(maxBoxes)
        {
            char boxPrice[32]; sprintf(boxPrice, " (%d$)", boxes[i].insideBox);
            char sellPrice[32]; sprintf(sellPrice, " - Selled for %d$", player.bankGain);
            char yourBox[32]; sprintf(yourBox, " (Your box)%s", player.bankGain ? sellPrice : "");

            printf("#####%d%s####%s%s\n", i+1, i+1>9 ? "" : "#", i+1==player.playerBox ? yourBox : "", boxes[i].opened ? boxPrice : "");
            printf("\n");
        }
    }

    void drawRemainingPrices()
    {
        int values[maxBoxes];
        int numValues = 0; // keep track of number of non-zero values

        loopi(maxBoxes)
        {
            if(!boxes[i].opened && boxes[i].insideBox != 0)
            {
                values[numValues++] = boxes[i].insideBox;
            }
        }

        sort(values, values + numValues); // sort only non-zero values

        if(numValues > 0)
        {
            conoutf(C_DEF, C_DEF, "Remaining boxes: %d\n", openCount(true));
            loopi(numValues)
            {
                conoutf(C_WHITE, values[i]<2000 ? C_BLUE : values[i]<20000 ? C_YELLOW : C_RED, "%d$", values[i]);
                if(i < numValues - 1) conoutf(C_DEF, C_DEF, " ");
            }
            conoutf(C_DEF, C_DEF, "\n");
        }
    }

    void playGame() // run a game
    {
        while(player.playerBox<1 || player.playerBox>maxBoxes)
        {
            printf("Please select a box from 1 to %d:\n", maxBoxes);
            scanf("%d", &player.playerBox);
        }
        clearConsole();

        for(;;)
        {
            drawGame();
            drawRemainingPrices();
            printf("Please choose a box to open:\n");
            scanf("%d", &player.choosenBox);
            if(player.choosenBox==player.playerBox)
            {
                printf("That's your box, please choose another one.\n");
                continue;
            }
            else if(boxes[player.choosenBox-1].opened)
            {
                printf("That box is already opened, please choose another one.\n");
                continue;
            }
            else if(player.choosenBox<1 || player.choosenBox>maxBoxes)
            {
                printf("Invalid box, please choose between 1 and %d.\n", maxBoxes);
                continue;
            }
            else
            {
                boxes[player.choosenBox-1].opened = true;

                bool setBankCall = false;
                switch(openCount()) { case 5: case 8: case 10: setBankCall = true; }

                if(!allOpened() && !setBankCall) clearConsole();
                printf("There was %d$ in the %d box!\n\n", boxes[player.choosenBox-1].insideBox, player.choosenBox);
                if(boxes[player.choosenBox-1].insideBox>=20000) sound::playSound("money_loss");
                else sound::playSound("box_open");
                if(setBankCall) bankCall();
            }

            if(allOpened())
            {
                clearConsole();
                if(player.bankGain) printf("You won %d$ and there was %d$ in your box, %s", player.bankGain, boxes[player.playerBox-1].insideBox, player.bankGain>=boxes[player.playerBox-1].insideBox ? "well done!\n\n" : "bad luck!\n\n");
                else printf("You won %d$ with your box %d.\n\n", boxes[player.playerBox-1].insideBox, player.playerBox);
                drawGame();
                break;
            }
        }
    }
}
