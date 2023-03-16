#include "main.h"

using namespace std;

namespace game
{
    //const int maxBoxes = 16;
    const int boxValues[maxBoxes] = {0, 1, 10, 30, 69, 100, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 250000, 500000};

    box boxes[maxBoxes];
    struct playerinfo player;

    void assignBoxes() // random distribution of boxes
    {
        random_device rd;
        mt19937 gen(rd());

        vector<int> tempBoxValues(boxValues, boxValues + maxBoxes);
        shuffle(tempBoxValues.begin(), tempBoxValues.end(), gen);

        loopi(maxBoxes)
        {
            boxes[i].opened = false;

            render::drawProgressBar((float)i / maxBoxes * 100);

            boxes[i].insideBox = tempBoxValues[i]; // assign the shuffled value directly to the boxes array
            clearConsole();
        }
    }

    void initGame() // (re)initialize everything for a new game
    {
        player.bankGain = 0;
        player.playerBox = 0;
        assignBoxes();
    }

    int openCount(bool remaining) // count the number of opened boxes (or the number of remaining)
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
        if(openCount(true)) bankOffer/= (openCount(true)<3 ? (float)openCount(true)*1.3 : (openCount(true)*3) + rnd(2)) ;

        if(player.bankGain)
        {
            printf("At this time, the bank would make another offer: %d$, %s", bankOffer, player.bankGain>=bankOffer ? "well done!\n\n" : "bad luck!\n\n");
        }
        else
        {
            bool exchangeOffer = !rnd(3);
            printf("The bank has an offer for you: %s%s Deal or no deal? (Y/N)\n", exchangeOffer ? "Box exchange," : to_string(bankOffer).c_str(), exchangeOffer? "" : "$");

            string response;
            while(response!="Y" || response!="N")
            {
                getline(cin, response);
                if(response=="Y")
                {
                    if(exchangeOffer)
                    {
                        printf("Please enter the box you want:\n");
                        playerInput(&player.playerBox);
                        clearConsole();
                        render::drawBoxes(player, boxes);
                        if(!allOpened()) render::drawRemainingPrices(player, boxes);
                        printf("Your box is now the number %d!:\n", player.playerBox);
                    }
                    else
                    {
                        player.bankGain = bankOffer;
                        sound::playSound("victory");
                        printf("You selled your box for %d$\n\n", player.bankGain);
                    }
                    break;
                }
                else if(response=="N")
                {
                    printf("You declined bank's offer.\n\n");
                        break;
                }
            }
        }
    }

    void playGame() // run a game
    {
        while(player.playerBox < 1 || player.playerBox > maxBoxes)
        {
            printf("Please select a box from 1 to %d:\n", maxBoxes);
            playerInput(&player.playerBox);
        }
        clearConsole();
        bool nextStep = true;

        for(;;)
        {
            if(nextStep)
            {
                render::drawBoxes(player, boxes);
                if(!allOpened()) render::drawRemainingPrices(player, boxes);
            }

            if(openCount() && nextStep) printf("There was %d$ in the %d box!\n", boxes[player.choosenBox-1].insideBox, player.choosenBox);

            if(allOpened())
            {
                if(player.bankGain) printf("You won %d$ and there was %d$ in your box, %s", player.bankGain, boxes[player.playerBox-1].insideBox, player.bankGain>=boxes[player.playerBox-1].insideBox ? "well done!\n\n" : "bad luck!\n\n");
                else printf("You won %d$ with your box %d.\n\n", boxes[player.playerBox-1].insideBox, player.playerBox);
                break;
            }

            if(nextStep) switch(openCount()) { case 6: case 10: case 14: bankCall(); }

            printf("Please choose a box to open:\n");
            playerInput(&player.choosenBox);

            if(player.choosenBox==player.playerBox)
            {
                printf("That's your box, please choose another one.\n");
                nextStep = false;
                continue;
            }
            else if(player.choosenBox<1 || player.choosenBox>maxBoxes)
            {
                player.choosenBox=0;
                printf("Invalid box, please choose between 1 and %d.\n", maxBoxes);
                nextStep = false;
                continue;
            }
            else if(boxes[player.choosenBox-1].opened)
            {
                printf("That box is already opened, please choose another one.\n");
                nextStep = false;
                continue;
            }
            else
            {
                boxes[player.choosenBox-1].opened = true;
                clearConsole();

                if(allOpened()) boxes[player.playerBox-1].opened = true;

                bool soundTrigger = (openCount() < 10 && boxes[player.choosenBox-1].insideBox>=50000) ? true :
                                     openCount() > 10 && boxes[player.choosenBox-1].insideBox>=10000 ? true : false;

                soundTrigger ? sound::playSound("money_loss") : sound::playSound("box_open");

                nextStep = true;
            }
        }
    }
}
