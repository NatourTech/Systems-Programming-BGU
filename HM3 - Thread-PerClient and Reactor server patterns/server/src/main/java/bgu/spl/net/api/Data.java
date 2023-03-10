package bgu.spl.net.api;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import bgu.spl.net.api.helperDataStructure.Pair;

public class Data {
    private static Data instance;
    private static ConcurrentHashMap<String,Pair<String,Boolean>> UsersPassData= new ConcurrentHashMap<>();
    private static ConcurrentHashMap<String, ConcurrentLinkedQueue<Pair<String,Integer>>> topics = new ConcurrentHashMap<>();
    /**
     * Retrieves the single instance of this class.
     */
    public static synchronized Data getInstance(){
        if (instance == null){
            instance = new Data();
        }
        return instance;
    }



    
    //Get Users data
    public ConcurrentHashMap<String,Pair<String,Boolean>>  getClietUserPass() {
        return UsersPassData;
    }

    //Get Topics and subscriber's data
    public ConcurrentHashMap<String, ConcurrentLinkedQueue<Pair<String,Integer>>> getTopics() {
        return topics;
    }

    public boolean isNewUser(String user){
        
        return UsersPassData.containsKey(user);

    }
    public boolean isUserLogedIn(String user , String pass){
        
        return (UsersPassData.get(user).getFirst().equals(pass));

    }

    public boolean isClientSubToTopic(String topic ,int connectionId){
        if(topics.containsKey(topic)){
            ConcurrentLinkedQueue<Pair<String,Integer>> topicSubcribers =  topics.get(topic);
            for (Pair<String,Integer> pair : topicSubcribers) {
                if(pair.getSecond() == connectionId ){
                    return true;
                }
                
              }
    
        }
       
        return false;
    }


    public static ConcurrentLinkedQueue<Pair<String,Integer>> getAllSubInTopic(String topic){

        return topics.get(topic);

    }
    public static boolean removeSubscription(String id , int connectionId){
        for (ConcurrentLinkedQueue<Pair<String,Integer>> value : topics.values()) {
            for (Pair<String,Integer> pair : value) {
                if(pair.getFirst().equals(id) && pair.getSecond() == connectionId){
                    value.remove(pair);
                    return true;
                }
            }
        }

        return false;
    }


    public static void removeAllClientSubscribtion(int connectionId){
        //remove all subscription for this client 
        for (ConcurrentLinkedQueue<Pair<String,Integer>> value : topics.values()) {
            for (Pair<String,Integer> pair : value) {
                if(pair.getSecond() == connectionId){
                    value.remove(pair);
                }
            }
        }
    }
    
    public static void changeUserStatus(String user){
        UsersPassData.get(user).setSecond(false);
    }
    

}
