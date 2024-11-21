// #include "observer.h"
#include "observer_implement.h"
int main(int argc, char** argv)
{

    Player player;

    AttackListener aListener;
    JumpListener jListener;

    player.addObserver(&aListener);
    player.addObserver(&jListener);

    player.notify(Event::Attack, "Player swing his sword!");    
    player.notify(Event::Attack, "Player swing his sword!");

    player.notify(Event::Jump, "Player jump!");
    
    player.notify(Event::Attack, "Player swing his sword!");    
    player.notify(Event::Jump, "Player jump!");

    return 0;
}