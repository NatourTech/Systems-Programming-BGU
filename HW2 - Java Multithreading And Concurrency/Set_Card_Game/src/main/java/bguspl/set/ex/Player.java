package bguspl.set.ex;

import java.util.*;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.logging.Level;

import bguspl.set.Env;

/**
 * This class manages the players' threads and data
 *
 * @inv id >= 0
 * @inv score >= 0
 */
public class Player implements Runnable {

    /**
     * The game environment object.
     */
    private final Env env;

    /**
     * Game entities.
     */
    private final Table table;

    /**
     * The id of the player (starting from 0).
     */
    public final int id;

    /**
     * The thread representing the current player.
     */
    public Thread playerThread;

    /**
     * The thread of the AI (computer) player (an additional thread used to generate key presses).
     */
    public Thread aiThread;
    //here it was private

    /**
     * True iff the player is human (not a computer player).
     */
    private final boolean human;

    private Dealer dealer;

    /**
     * True iff game should be terminated due to an external event.
     */
    private volatile boolean terminate;

    /**
     * The current score of the player.
     */
    private int score;

    public volatile  boolean in;

    public volatile long FreezingTime;

    //We Added
    public volatile List<Integer> tokens = new LinkedList<>();


    /**
     * The class constructor.
     *
     * @param env    - the environment object.
     * @param dealer - the dealer object.
     * @param table  - the table object.
     * @param id     - the id of the player.
     * @param human  - true iff the player is a human player (i.e. input is provided manually, via the keyboard).
     */
    public Player(Env env, Dealer dealer, Table table, int id, boolean human) {
        this.env = env;
        this.table = table;
        this.id = id;
        this.human = human;
        this.dealer = dealer;
        in = true;
    }

    /**
     * The main player thread of each player starts here (main loop for the player thread).
     */
    @Override
    public void run() {
        playerThread = Thread.currentThread();
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + "starting.");
        if (!human) createArtificialIntelligence();
        while (!terminate) {
            if (this.tokens.size() == 3 && in && (human || aiThread.getState() == Thread.State.WAITING )) {
                addPlayerToLegal(this);
                synchronized (this) {
                        try {
                            wait();
                        for (long i = FreezingTime; i > 0; i = i - 1000) {
                            env.ui.setFreeze(this.id, i);
                            Thread.sleep(1000);
                        }
                        env.ui.setFreeze(this.id, 0);
                            if(!human) {
                                wakeAI();
                            }
                        } catch (InterruptedException ex) {
                            wakeAI();
                        }
                }

            }
        }
        if (!human) {wakeAI(); try { aiThread.join(); } catch (InterruptedException ignored) {}}
        env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
    }

    /**
     * Creates an additional thread for an AI (computer) player. The main loop of this thread repeatedly generates
     * key presses. If the queue of key presses is full, the thread waits until it is not full.
     */
    private void createArtificialIntelligence() {
        // note: this is a very very smart AI (!)
        aiThread = new Thread(() -> {
            aiThread.setName("AiThread of Player.id: " + this.id);
            env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " starting.");
            while (!terminate) {
                while (tokens.size() != 3 || !in) {
                    int rand = (int) (Math.random() * dealer.gridSize);
                    keyPressed(rand);
                }
                synchronized (aiThread) {
                    try {
                        aiThread.wait();
                   } catch (InterruptedException ex) {
                        if(this.terminate)break;
                        continue;
                    }
                }
            }
            env.logger.log(Level.INFO, "Thread " + Thread.currentThread().getName() + " terminated.");
        }, "computer-" + id);
        aiThread.start();
    }

    /**
     * Called when the game should be terminated due to an external event.
     */
    public void terminate() {
        this.terminate = true;
    }

    /**
     * This method is called when a key is pressed.
     *
     * @param slot - the slot corresponding to the key pressed.
     */
    public void keyPressed(int slot) {
        if(playerThread.getState() == Thread.State.RUNNABLE && table.slotToCard[slot] != null && dealer.canPlace){

            if (tokens.contains(slot)) {
                table.removeToken(this.id, slot);
                tokens.remove(Integer.valueOf(slot));
                in = true;
            } else if (this.tokens.size() < 3) {
                this.tokens.add(slot);
                table.placeToken(this.id, slot);
            }
        }

    }

    /**
     * Award a point to a player and perform other related actions.
     *
     * @post - the player's score is increased by 1.
     * @post - the player's score is updated in the ui.
     */
    public void point() {
        synchronized (this) {
            int ignored = table.countCards(); // this part is just for demonstration in the unit tests
            env.ui.setScore(id, ++score);
            if(score > dealer.HiScore) {dealer.HiScore = score; dealer.noPlayersHi = 1;}
            else if( score == dealer.HiScore) dealer.noPlayersHi++;
            FreezingTime = env.config.pointFreezeMillis;
            notify();
        }
    }

    /**
     * Penalize a player and perform other related actions.
     */
    public void penalty() {
        synchronized (this) {
            FreezingTime = env.config.penaltyFreezeMillis;
            in = false;
            removePenlzdPlayer(this);
            notify();
        }
    }

    public int score() {
        return score;
    }
    //We Added:Sync add to legal set

    public synchronized void addPlayerToLegal(Player player){
            this.table.addPlayerToLegal(player);
    }

    public synchronized void removePenlzdPlayer(Player toremove){
        this.table.removePlayer(toremove);
    }

    public void removeAllTokens(){
        for(int i =0;i<tokens.size();i++){
            table.removeToken(this.id,tokens.remove(i));
        }
        // tokens.clear();
    }

    public int [] convertToCards(){
        int [] cards = new int[tokens.size()];
        for(int i = 0 ; i < tokens.size(); i++){
            cards[i] = -1;
            if(table.slotToCard[tokens.get(i)] != null)
            cards[i] = table.slotToCard[tokens.get(i)];
        }
        return cards;
    }

    public void removeIfExist(int slot){
        //here happened index out of bounds when turntimeoutmillis = 0........!@#!@#!@#!@#
        if(tokens.contains(slot)){
            in = true;
            //here
            //simply remove it instead of index?
            //tokens.remove(slot);
            tokens.remove(tokens.indexOf(slot));
            table.removeToken(this.id,slot);
            // table.removePlayer(this);
        }
    }

    public void wake(){
        synchronized (this){
            FreezingTime = 0;
            notify();
        }
    }

//    public void smartPick(){
//        int[] picks = dealer.findGridSetSlots();
//        System.out.println(picks.length);
//        for (int pick: picks) {
//            try{
//                Thread.sleep(500);
//            }catch (InterruptedException ex){}
//            keyPressed(pick);
//        }
//
//    }

    public void randomPick(){
        for(int i= 0 ; i<3;i++){
            int rand = (int) (Math.random()*12);
            try{
                Thread.sleep(500);
            }catch (InterruptedException ex){}
            keyPressed(rand);
        }
    }

    public void AiRemoveTokens(){
            while (!tokens.isEmpty()) {
                keyPressed(tokens.get(0));
            }
    }

    public void wakeAI(){
        synchronized (aiThread){
            aiThread.interrupt();
        }
    }

}
