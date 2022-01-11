import java.util.*;

public class MedalTable {

    // Creating a MedalCountry class
    private class MedalCountry {
        String myName;
        int myGold;
        int mySilver;
        int myBronze;

        //constructor
        MedalCountry(String name) {
            myName = name;
            myGold = 0;
            mySilver = 0;
            myBronze = 0;
        }

        //get methods
        public int getGold() {
            return myGold;
        }
        public int getSilver() {
            return mySilver;
        }
        public int getBronze() {
            return myBronze;
        }
        public String getName() {
            return myName;
        }

        //new toString()
        @Override
        public String toString() {
            return String.format("%s %d %d %d", myName, myGold, mySilver, myBronze);
        }
    }
    public String[] generate(String[] results) {
        HashMap <String, MedalCountry> map = new HashMap<>();

        // Adding all countries and medals to table
        for (String s : results) {
            String [] data = s.split(" ");
            for (int i = 0; i<data.length; i++) {

                // Creating new country object if not seen
                if (!map.containsKey(data[i])) {
                    map.put(data[i], new MedalCountry(data[i]));
                }

                // Adding medal count per country
                if (i == 0) map.get(data[i]).myGold++;
                if (i == 1) map.get(data[i]).mySilver++;
                if (i == 2) map.get(data[i]).myBronze++;
            }
        }

        // Sorting table correctly according to directions
        ArrayList<MedalCountry> list = new ArrayList<>(map.values());
        list.sort(Comparator.comparing(MedalCountry::getName));
        list.sort(Collections.reverseOrder(Comparator.comparing(MedalCountry::getBronze)));
        list.sort(Collections.reverseOrder(Comparator.comparing(MedalCountry::getSilver)));
        list.sort(Collections.reverseOrder(Comparator.comparing(MedalCountry::getGold)));
        String [] ret = new String [list.size()];
        int i = 0;
        for (MedalCountry mc : list) {
            ret[i] = mc.toString();
            i++;
        }
        return ret;
    }

}

