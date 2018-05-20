/**
 * Author: Tsz-Chiu Au and Olzhas Kaiyrakhmet (Nickname: olzhabay) 
 * Email: chiu@unist.ac.kr and olzhabay.i@gmail.com 
 */

import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.LinkedList;
import java.util.Random;
import java.util.Scanner;

public class Maze {

    private static final String DEFAULT_FILE_NAME = "map.out";
    private static final double HOLE_FACTOR = 0.08;
    private static final double START_FINISH_DISTANCE_LEAST_RATIO = 0.3;
    
    public static final Random rand = new Random();

    private boolean[][] wall; // true means wall
    private int sizeX;
    private int sizeY;
    private int startX;
    private int startY;
    private int finishX;
    private int finishY;
    private int actualShortestDistance;

    public Maze(int x, int y) {
        generateMap(x, y);
        writeMap(DEFAULT_FILE_NAME);
    }
    
    public Maze(String fileName) {
        readMap(fileName);
    }

    public boolean isWall(int x, int y) {
        return wall[y][x];
    }
            
    public int getSizeX(){
        return sizeX;
    }

    public int getSizeY(){
        return sizeY;
    }

    public int getStartX(){
        return startX;
    }

    public int getStartY(){
        return startY;
    }
    
    public int getFinishX(){
        return finishX;
    }

    public int getFinishY(){
        return finishY;
    }
    
    public int getActualShortestDistance() {
        return actualShortestDistance;
    }
    
    public boolean hasSouthWall(int x, int y) {
        return (y >= sizeY-1) || wall[y+1][x];
    }

    public boolean hasNorthWall(int x, int y) {
        return (y <= 0) || wall[y-1][x];
    }

    public boolean hasEastWall(int x, int y) {
        return (x >= sizeX-1) || wall[y][x+1];
    }

    public boolean hasWestWall(int x, int y) {
        return (x <= 0) || wall[y][x-1];
    }

    public double getEuclideanDistance(int x, int y) {
        return Math.sqrt(Math.pow((finishX-x),2)+Math.pow((finishY-y),2));
    }

    private void readMap(String fileName) {
        try (Scanner in = new Scanner(new FileReader(fileName))) {
            if (in.hasNext()) {
                sizeX = in.nextInt();
                sizeY = in.nextInt();
                actualShortestDistance = in.nextInt();
                startX = in.nextInt();
                startY = in.nextInt();
                finishX = in.nextInt();
                finishY = in.nextInt();
                wall = new boolean[sizeY][sizeX];
                for (int y = 0; y < sizeY; y++)
                    for (int x = 0; x < sizeX; x++)
                        wall[y][x] = (in.nextInt() == 1);
            }
        } catch (Exception e) {
            System.err.println("Cannot read from " + fileName);
            System.exit(1);
        }
    }

    public void writeMap(String fileName) {//procedure to print generated map to file
        try (PrintWriter writer = new PrintWriter(new FileWriter(fileName))) {
            writer.println(sizeX + " " + sizeY);
            writer.println(actualShortestDistance);
            writer.println(startX + " " + startY);
            writer.println(finishX + " " + finishY);
            for (int y = 0; y < sizeY; y++) {
                for (int x = 0; x < sizeX; x++) {
                    writer.print((wall[y][x]?1:0) + " ");
                }
                writer.println();
            }
        } catch (Exception e) {
            System.err.println("Cannot write to " + fileName);
            System.exit(1);
        }
    }

    private void generateMap(int x, int y) {
        sizeX = x;
        sizeY = y;
        wall = new boolean[sizeY][sizeX];
        for (int i = 0; i < sizeY; i++) {
            for (int j = 0; j < sizeX; j++) {
                wall[i][j] = true;
            }
        }

        mazeGenByDFS(1, 1);
        randomHoles();
        
        //generator start and finish positions
        while (wall[startY][startX] || wall[finishY][finishX] ||
               (Math.abs(startY - startX) + Math.abs(finishY - finishX)) <= ((sizeX + sizeY) * START_FINISH_DISTANCE_LEAST_RATIO / 2)) {
            startX = rand.nextInt(sizeX);
            startY = rand.nextInt(sizeY);
            finishX = rand.nextInt(sizeX);
            finishY = rand.nextInt(sizeY);
        }

        actualShortestDistance = findShortestDistanceByBFS(startX, startY, finishX, finishY);
        
        assert actualShortestDistance >= 0;
    }

    private boolean isLoopAllowed(int x, int y) {
        if (!wall[y][x]) {
            return false;
        }
        if (y == 0 || x == 0 || y == sizeY - 1 || x == sizeX - 1) {
            return false;
        }
        if (!wall[y + 1][x] && !wall[y - 1][x] && wall[y][x + 1] && wall[y][x - 1]) {
            return true;
        }
        if (wall[y + 1][x] && wall[y - 1][x] && !wall[y][x + 1] && !wall[y][x - 1]) {
            return true;
        }
        return false;
    }

    private void mazeGenByDFS(int y, int x) {//randomized DFS(recursion)
        wall[y][x] = false;
        while (true) {
            int direction = rand.nextInt(4);//Random direction generation
            if ((direction == 0) && checkEast(x, y)) {
                //Right Direction
                mazeGenByDFS(y, x + 1);
            } else if ((direction == 1) && checkWest(x, y)) {
                //Left Direction
                mazeGenByDFS(y, x - 1);
            } else if ((direction == 2) && checkSouth(x, y)) {
                //Down Direction
                mazeGenByDFS(y + 1, x);
            } else if ((direction == 3) && checkNorth(x, y)) {
                //Up Direction
                mazeGenByDFS(y - 1, x);
            } else if (!checkNorth(x, y) && !checkSouth(x, y) && !checkWest(x, y) && !checkEast(x, y)) {
                //no Direction
                break;
            }
        }
    }

    private boolean checkEast(int x, int y) {//checking possibility for going by DFS to Right
        if ((x + 1 < sizeX - 1) && wall[y][x + 1] && wall[y][x + 2]
                && wall[y + 1][x + 1] && wall[y - 1][x + 1]
                && wall[y + 1][x + 2] && wall[y - 1][x + 2]) {
            return true;
        } else {
            return false;
        }
    }

    private boolean checkWest(int x, int y) {  //checking possibility for going by DFS to Left
        if ((x - 1 > 0) && wall[y][x - 1] && wall[y][x - 2]
                && wall[y + 1][x - 1] && wall[y - 1][x - 1]
                && wall[y + 1][x - 2] && wall[y - 1][x - 2]) {
            return true;
        } else {
            return false;
        }
    }

    private boolean checkSouth(int x, int y) {//checking possibility for going by DFS to Down
        if ((y + 1 < sizeY - 1) && wall[y + 1][x] && wall[y + 2][x]
                && wall[y + 1][x + 1] && wall[y + 1][x - 1]
                && wall[y + 2][x + 1] && wall[y + 2][x - 1]) {
            return true;
        } else {
            return false;
        }
    }

    private boolean checkNorth(int x, int y) {//checking possibility for going by DFS to Up
        if ((y - 1 > 0) && wall[y - 1][x] && wall[y - 2][x]
                && wall[y - 1][x + 1] && wall[y - 1][x - 1]
                && wall[y - 2][x + 1] && wall[y - 2][x - 1]) {
            return true;
        } else {
            return false;
        }
    }
    
    private void randomHoles() {
        for(int i=0; i < sizeX * sizeY * HOLE_FACTOR ; i++) {
            int x = rand.nextInt(sizeX-2) + 1;
            int y = rand.nextInt(sizeY-2) + 1;
            wall[y][x] = false;
        }
    }

    private class Node {
        int x, y;
        int level;
        Node parent;
        
        public Node(int x, int y, int level, Node parent) {
            this.x = x;
            this.y = y;
            this.level = level;
            this.parent = parent;
        }
        
        public int getX() { return x; }
        public int getY() { return y; }
        public int getLevel() { return level; }
        public Node getParent() { return parent; }
    }

    private int findShortestDistanceByBFS(int x1, int y1, int x2, int y2) {
        LinkedList<Node> queue = new LinkedList<>();
        boolean[][] visited = new boolean[sizeX][sizeY];
        
        for(int y=0; y<sizeY; y++)
            for(int x=0; x<sizeX; x++)
                visited[x][y] = false;

        Node node = new Node(x1, y1, 0, null);
        queue.add(node);
        visited[x1][y1] = true;
        
        while(!queue.isEmpty()) {
            node = queue.removeFirst();
            
            if (node.getX() == x2 && node.getY() == y2) {
                return node.getLevel();
            }
                    
            int x = node.getX();
            int y = node.getY();
            if (!hasWestWall(x, y) && !visited[x-1][y]) {
               queue.add(new Node(x-1, y, node.getLevel()+1, node));
               visited[x-1][y] =  true;
            }
            if (!hasEastWall(x, y) && !visited[x+1][y]) {
               queue.add(new Node(x+1, y, node.getLevel()+1, node));
               visited[x+1][y] =  true;
            }
            if (!hasNorthWall(x, y) && !visited[x][y-1]) {
               queue.add(new Node(x, y-1, node.getLevel()+1, node));
               visited[x][y-1] =  true;
            }
            if (!hasSouthWall(x, y) && !visited[x][y+1]) {
               queue.add(new Node(x, y+1, node.getLevel()+1, node));
               visited[x][y+1] =  true;
            }
        }
        
        return -1; // no solution
    }


}
