package bguspl.set.ex;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import bguspl.set.Config;
import bguspl.set.Env;
import bguspl.set.UserInterface;
import bguspl.set.Util;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.logging.Logger;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.eq;


import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class DealerTest {

    Dealer dealer;

    @Mock
    Util util;
    @Mock
    private UserInterface ui;
    @Mock
    private Table table;
    @Mock
    private Logger logger;
    @Mock
    private Env env;
    private Player [] players;



    @BeforeEach
    void setUp() {
        env = new Env(logger, new Config(logger, (String) null), ui, util);
        players = new Player [2];
        dealer = new Dealer(env,table,players);
        for (int i = 0 ; i<players.length;i++){
            players[i] = new Player(env,dealer,table,i,true);
        }
    }

    private void allowAllout(){
        for (Player p:players) {
            p.in = false;
        }
    }

    @Test
    void resetTimer() {
        long currTime = dealer.time();
        try {
            Thread.sleep(200);
        }catch (InterruptedException ex){}
        dealer.resetTimer();
        long curr2 = dealer.time();
        long diffTime1 = (curr2 - currTime);
        long curr3 = dealer.time();
        assertTrue(currTime != curr3);
        try {
            Thread.sleep(200);
        }catch (InterruptedException ex){}
        dealer.resetTimer();
        long curr4 = dealer.time();
        long diffTime2 = (curr4 - currTime);
        assertTrue(diffTime2>diffTime1 ,"diffTime2 should be bigger than diffTime1");
    }


    @Test
    void allowAllIn() {
        allowAllout();
        dealer.allowAllIn();
        for (Player p:players) {
            assertTrue(p.in,"in should be true");
        }
    }
}