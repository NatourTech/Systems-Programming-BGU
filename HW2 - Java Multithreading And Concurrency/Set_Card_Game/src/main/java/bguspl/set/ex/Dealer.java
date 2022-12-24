package bguspl.set.ex;

import bguspl.set.Env;

import java.util.*;
import java.util.logging.Level;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/**
 * This class manages the dealer's threads and data
 */
public class Dealer implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;
    Thread timer;

    public volatile int HiScore;
    public volatile int noPlayersHi;
    /**
     * Game entities.
     */
    private final Table table;
    private final Player[] players;

    public volatile boolean canPlace;
    /**
     * The list of card ids that are left in the dealer's deck.
     */
    private final List<Integer> deck;
    private final List<Integer> gridCards;

    /**
     * True iff game should be terminated due to an external event.
     */
    private volatile boolean terminate;

    private volatile boolean elapsed;

    private long confElapsed;

    public int gridSize;

    private Object locker = new Object();
    /**
     * The time when the dealer needs to reshuffle the deck due to turn timeout.
     */
    private long countDown ;


    private List<Thread> threads;


    public Dealer(Env env, Table table, Player[] players) {
        this.HiScore = 0;
        this.env = env;
        this.table = table;
        this.players = players;
        this.threads = new ArrayList<>();
        confElapsed=0;
        deck = IntStream.range(0, env.config.deckSize).boxed().collect(Collectors.toList());
        gridSize = (env.config.columns*env.config.rows);
        gridCards = new ArrayList<>(gridSize);
        this.countDown = System.currentTimeMillis();
        canPlace = false;
        elapsed = (env.config.turnTimeoutMillis <= 0);
        createTimer(elapsed);
    }

    /**
     * The dealer thread starts here (main loop for the dealer thread).
     */
    @Override
    public void run() {
        placeCardsOnTable();
        playerStart();
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
        this.countDown = System.currentTimeMillis();
        timer.start();
        while (!shouldFinish()) {
            ElapsedReshuffle();
            checkLegalSets();
        }

        terminate = true;
        terminateAll();
        try{
            Thread.sleep(200);
            removeAllCardsFromTable();
            Thread.sleep(500);
            announceWinners();
        }catch (InterruptedException ex){}

        Collections.reverse(threads);
            for (Thread thd : threads) {
                    if (terminate) {
                        thd.interrupt();
                        try {thd.join();}
                        catch (InterruptedException ex){}
                    }
            }
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * Called when the game should be terminated due to an external event.
     */
    public void terminate() {
        this.terminate = true;
        System.exit(0);
    }

    /**
     * Check if the game should be terminated or the game end conditions are met.
     *
     * @return true iff the game should be finished.
     */
    private boolean shouldFinish() {
        synchronized (locker) {
            return terminate || (env.util.findSets(deck, 1).size() == 0 && env.util.findSets(gridCards, 1).size() == 0);
        }
    }

    /**
     * Check if any cards can be removed from the deck and placed on the table.
     */
    private void placeCardsOnTable() {
        canPlace = false;
        countDown= System.currentTimeMillis();
        shuffleDeck();
        for(int i = 0; i < gridSize && deck.size()>0; i++) {
            //removing cards from deck
            int random = (int) (Math.random() * deck.size());
            Integer currCard = this.deck.remove(random);
            this.gridCards.add(currCard);
            this.table.placeCard(currCard, i);
        }
        canPlace = true;
    }

    /**
     * Returns all the cards from the table to the deck.
     */
    private void removeAllCardsFromTable() {
        canPlace = false;
        for(int i = 0; i<table.slotToCard.length && gridCards.size() > 0;i++){
            this.deck.add(gridCards.remove(0));
            for(int j =0 ; j<players.length;j++){
                table.removeToken(j,i);
                players[j].removeAllTokens();
            }
        }
        for(int i = 0; i<table.slotToCard.length;i++){
            table.removeCard(i);
        }
    }

    /**
     * Check who is/are the winner/s and displays them.
     */
    private void announceWinners() {
        ArrayList<Integer> ps = new ArrayList<>();
        for (Player curr: players) {
           if(curr.score() == HiScore) ps.add(curr.id);
        }
        int [] players = new int [ps.size()];
        players = ps.stream().mapToInt(i-> i).toArray();
        env.ui.announceWinner(players);
    }


    private void fillSlots(){
        for(int i = 0 ; i <gridSize;i++){
            if(table.slotToCard[i] == null && deck.size() > 0){
                int a =(int) (Math.random() * (deck.size()));
                Integer currCard = deck.remove(a);
                gridCards.add(currCard);
                table.placeCard(currCard,i);
                try {
                    Thread.sleep(env.config.tableDelayMillis);
                }catch (InterruptedException ex){}
            }
        }
    }


    private void shuffleDeck(){
        Collections.shuffle(this.deck);
    }


    private void playerStart(){
        for(Player player : this.players ){
            Thread playerThread = new Thread(player);
            playerThread.setName("Player Thread.id: " + player.id);
            playerThread.start();
            threads.add(playerThread);
        }
    }

    private void checkLegalSets(){
        if (table.PlayersLegalSets.size() > 0) {
            Player curr = table.PlayersLegalSets.poll();
            int[] playerCrads = curr.convertToCards();
            if (playerCrads.length == 3) {
                checkNull(curr,playerCrads);
                if (env.util.testSet(playerCrads)) {
                    resetTimer();
                    for (int i = 0; i < playerCrads.length; i++) {
                        for (Player p : players) {
                            p.removeIfExist(table.cardToSlot[playerCrads[i]]);
                        }
                        table.removeCard(table.cardToSlot[playerCrads[i]]);
                        this.gridCards.remove(Integer.valueOf(playerCrads[i]));
                    }
                    fillSlots();
                    curr.point();
                } else {
                    curr.penalty();
                }
            } else {
                curr.wake();
            }
        }
    }

    public void resetTimer(){
        if(!elapsed) {
            this.countDown = System.currentTimeMillis();
        }
        else{
            env.ui.setElapsed(confElapsed);
        }
    }

    public  void reshuffleAftertimer(){
        synchronized (locker) {
            shuffleDeck();
            removeAllCardsFromTable();
            placeCardsOnTable();
            resetTimer();
            allowAllIn();
        }
    }
    public void allowAllIn(){
        for (Player a: players) {
            a.in = true;
        }
    }


    //this function was meant to be used to make a smart pick in the AI mode but We didn't use it 
    
    // public List<int []> findGridSetSlots(){
    //     List<int []> set = env.util.findSets(gridCards,20);
    //     int[] slots = new int[3];
    //     if(set.size() == 0) {
    //         for (int j = 0; j < set.size(); j++) {
    //             slots[j] = table.cardToSlot[set];
    //         }
    //     }
    //     return set;
    // }


    public void ElapsedReshuffle(){
        if(elapsed){
            if(env.util.findSets(gridCards,1).size() == 0){
                reshuffleAftertimer();
            }
        }
    }

    private void createTimer(boolean elapsed){
        if(!elapsed){
            timer = new Thread(){
                public void run(){
                    env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
                    while(!terminate) {
                        long elapsedTime =  (env.config.turnTimeoutMillis+1000)+(countDown - System.currentTimeMillis());
                        try {
                            Thread.sleep(10);
                        } catch (InterruptedException ex) {}

                        if(elapsedTime<=0){env.ui.setCountdown(0,true);
                            reshuffleAftertimer();}
                        env.ui.setCountdown(elapsedTime, elapsedTime<=env.config.turnTimeoutWarningMillis);

                    }
                    try {
                        Thread.sleep(1000);
                    }catch (InterruptedException ex){}
                    env.ui.setCountdown(0,false);
                    env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
                }

            };
        }
        else {

            timer = new Thread(){
                public void run(){
                    env.ui.setElapsed(0);
                    env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
                    while(!terminate && elapsed){
                        long elapsedTime = (System.currentTimeMillis() - countDown);
                        if(env.config.turnTimeoutMillis == 0) {confElapsed = elapsedTime;} //if what I understood first then ==>  //{env.ui.setElapsed(elapsedTime + 500);}
                        else {threads.remove(timer);break;}}
                    env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
                }
            };
        }
        threads.add(timer);
        timer.setName("Timer Thread");
    }

    public void checkNull(Player curr,int [] playerCards){
        for(int i = 0 ; i<playerCards.length;i++){
            if (playerCards[i] == -1){
                curr.removeAllTokens();
                curr.in = true;
                curr.wake();
                break;
            }
        }

    }

    private void terminateAll(){
        for (Player p: players) {
            p.terminate();
        }
    }

    //for test
    public long time(){
        return countDown;
    }


}
