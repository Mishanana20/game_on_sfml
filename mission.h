///////////////////////////////////????? ??????//////////////////////////////////
#include <SFML/Audio.hpp>
int getCurrentMission(int x,int y)//?-??? ?????? ??????, ??????? ?????? ????? ??????, ? ??????????? ?? ?????????? ?????? ? (???? ????? ?????????? ??? ??????????)
{
    int mission = 0;
    if ((x >545) && (y > 652) && ((x < 1261) && (y < 983))) { mission = 1; } //???????? ?????? ? ?????
    if ((x > 790) && (y > 267) && ((x < 1440) && (y < 600))) { mission = 2;} //????? ?? ?????? ??????
    if ((x > 1376) && (y > 743) && ((x < 1761) && (y < 983))) { mission = 3; }//2??
    if ((x > 2752) && (y > 297) && ((x < 3628) && (y < 628))) { mission = 4; }//? ??
    if ((x > 4765) && (y > 852) && ((x <5246) && (y <957))) { mission =5; }//Boss
    if ((x > 3107)&&(y>1276)&&((x<3609)&&(y<1583))) { mission = 6;}//Boss
    if ((x > 389) && (y > 1025) && ((x < 2622) && (y < 1431))) { mission = 7; }//Boss
    if ((x > 5255) && (y > 1575) && ((x < 5823) && (y < 2091))) { mission = 8; }//Boss
    if ((x >3589) && (y >1573) && ((x < 5187) && (y < 1997))) { mission = 9; }//Boss


    return mission;//?-??? ?????????? ????? ??????
}



/////////////////////////////////////????? ??????/////////////////////////////////
std::string getTextMission(int currentMission) {
    std::string missionText = "";//????? ?????? ? ??? ?????????????

    switch (currentMission)//??????????? ????? ?????? ? ? ??????????? ?? ???? ?????????? missionText ????????????? ????????? ?????
    {
    case 0: missionText = " ????=50\n\n?????-?????... \n???! \n?? ???????????!\n???? \n?????? ????"; break;
    case 1: missionText = " ????=50 \n???-?? ?????????? \n???????? \n?????? ???????. \n??? ? ???? \n?????? ?????????! \n\n ????:\n ????????=250,\n ???? = 1 (???????)\n ???????? = 15\n ??????????? = ????? "; break;
    case 2: missionText = " ????=50\n?????????\n????????...\n? ????? ?????? ???? \n?????? ? ????????\n\n'????????':\n????????=200,\n???? = 50\n????????=10\n???????????:???????? \n???? ??????"; break;
    case 3: missionText = " ????=50\n\n?????????!... \n??? ??? ????????\n???? ????? \n?????? ????"; break;
    case 4: missionText = " ????=50\n\n??! ??????! \n????? ?? ????? \n??? ???? ?? \n????????"; break;
    case 5: missionText = " ????=50\n\n??? ? ???? \n? ??????????.\n???? ??????????!"; break;
    case 6: missionText = " ????=50\n \n?????????? ?????????\n? ?????? ????????.\n\n??????? ?????\n????????? ???\n???? ?????."; break;
    case 7: missionText = " ????=50\n \n? ??? ?????????\n\n?? ????? ????\n??? ???!"; break;
    case 8: missionText = " \n\n???????? ????????!\n??????? ??? ?????\n?????? ???????\n???? ??????? \n?????????\n?? ??? ??? ?? ?????\n????? ?????\n?? ??? ??????"; break;
    case 9: missionText = " ????=50\n\n????????!\n????????=750!\n?????=20\n???????????:\n????? ???????.\n???????? ?????."; break;
    }

    return missionText;//?-??? ?????????? ?????
};///////////??? ???? 14. ????? ??????????