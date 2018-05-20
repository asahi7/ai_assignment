/**
 * Author: Tsz-Chiu Au and Olzhas Kaiyrakhmet (Nickname: olzhabay) 
 * Email: chiu@unist.ac.kr and olzhabay.i@gmail.com 
 */

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedList;
import java.util.List;


class MazeFrame extends JFrame {

    private static final int DEFAULT_SIZE_X = 120;
    private static final int DEFAULT_SIZE_Y = 80;
            
    private Maze maze;
    
    private List<Coordinate> visitedPos;
    private boolean[][] visitedPosMap;
    private List<Coordinate> shortestPath;
    private boolean[][] shortestPathMap;
    
    
    private class MazePanel extends JPanel {

        private static final int CELL_SIZE = 5;
        private static final int PANEL_WIDTH = 900;
        private static final int PANEL_HEIGHT = 700;
        
        private static final int PANEL_ORIGIN_X = 25;
        private static final int PANEL_ORIGIN_Y = 25;
        private static final int POINT_RADIUS = 10;
        

        @Override
        protected void paintComponent(Graphics g){
            super.paintComponent(g);
            Graphics2D g2 = (Graphics2D) g;
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

            int yRect = PANEL_ORIGIN_Y;
            Rectangle cell;
            for (int y = 0; y < maze.getSizeY(); y++) {
                int xRect = PANEL_ORIGIN_X;
                for (int x = 0; x < maze.getSizeX(); x++) {
                    g2.setPaint(getPositionColor(x, y));
                    cell = new Rectangle(xRect, yRect, CELL_SIZE, CELL_SIZE);
                    g2.fill(cell);
                    xRect += CELL_SIZE;
                }
                yRect += CELL_SIZE;
            }
            g2.setPaint(Color.blue);
            g2.drawOval(PANEL_ORIGIN_X + maze.getStartX() * CELL_SIZE + CELL_SIZE/2 - POINT_RADIUS,
                        PANEL_ORIGIN_Y + maze.getStartY() * CELL_SIZE + CELL_SIZE/2 - POINT_RADIUS,
                        POINT_RADIUS * 2, POINT_RADIUS * 2);
            g2.setPaint(Color.red);
            g2.drawOval(PANEL_ORIGIN_X + maze.getFinishX() * CELL_SIZE + CELL_SIZE/2 - POINT_RADIUS,
                        PANEL_ORIGIN_Y + maze.getFinishY() * CELL_SIZE + CELL_SIZE/2 - POINT_RADIUS,
                        POINT_RADIUS * 2, POINT_RADIUS * 2);
            
            if (shortestPath.size() > 0) {
                int memory = (int) Math.log10(Runtime.getRuntime().maxMemory() / 1024.0 / 1024.0) + 1;
                g2.setPaint(Color.red);
                g2.drawString("Actual shortest distance = " + maze.getActualShortestDistance() +
                              ", The shortest distance you found = " + (shortestPath.size()-1) +
                              ", Number of visited spaces  = " + visitedPos.size() +
                              ", Memory usage = " + memory + "MB", 10, 16);
            } else {
                g2.setPaint(Color.red);
                g2.drawString("Actual shortest distance = " + maze.getActualShortestDistance(), 10, 16);
            }
        }
        
        private Color getPositionColor(int x, int y) {
            if (x == maze.getStartX() && y == maze.getStartY()) {
                return Color.blue;
            } else if (x == maze.getFinishX() && y == maze.getFinishY()) {
                return Color.red;
            } else if (shortestPathMap[x][y]) {
                assert visitedPosMap[x][y];
                return Color.orange;
            } else if (visitedPosMap[x][y]) {
                return Color.green;
            } else if (maze.isWall(x, y)) {
                return Color.DARK_GRAY;
            } else { // empty space
                return Color.white;
            }
        }
        
        @Override
        public Dimension getPreferredSize(){//no comment)
            return new Dimension(PANEL_WIDTH, PANEL_HEIGHT);
        }

    }

    public MazeFrame() {
        maze = new Maze(DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
        setup();
    }
    
    public MazeFrame(String fileName) {
        maze = new Maze(fileName);
        setup();
    }

    private void setup() {
        resetState();
        
        //maze graphic component
        final MazePanel mazePanel = new MazePanel();

        //Height,Width Fields
        final JTextField fieldWidth = new JTextField(Integer.toString(maze.getSizeX()));
        final JTextField fieldHeight = new JTextField(Integer.toString(maze.getSizeY()));

        //Generate Button Button
        JButton generateButton = new JButton("Generate new maze");
        generateButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                int xNumCells = Integer.parseInt(fieldWidth.getText());
                int yNumCells = Integer.parseInt(fieldHeight.getText());
                maze = new Maze(xNumCells, yNumCells);
                resetState();
                repaint();
            }
        });

        //Clear Map Button
        JButton clearMapButton = new JButton("Clear Map");
        clearMapButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                resetState();
                repaint();
            }
        });

        //panel with generation button and fields
        JPanel generationPanel = new JPanel();
        generationPanel.setLayout(new GridLayout(6, 1));
        generationPanel.add(new JLabel("Width :", SwingConstants.LEFT));
        generationPanel.add(fieldWidth);
        generationPanel.add(new JLabel("Height :", SwingConstants.LEFT));
        generationPanel.add(fieldHeight);
        generationPanel.add(generateButton);
        generationPanel.add(clearMapButton);
        generationPanel.setBorder(BorderFactory.createEtchedBorder());

        //check box and radio buttons
        final JRadioButton aStarButton = new JRadioButton("A-star search", true);
        final JRadioButton dfsButton = new JRadioButton("DFS search", false);
        final JRadioButton bfsButton = new JRadioButton("BFS search", false);

        ButtonGroup searchButtonGroup = new ButtonGroup();
        searchButtonGroup.add(aStarButton);
        searchButtonGroup.add(dfsButton);
        searchButtonGroup.add(bfsButton);

        //Start Rat button
        JButton startAgentButton = new JButton("Start Agent");
        startAgentButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if (aStarButton.isSelected()) {
                    startAgent(SearchMode.ASTAR);
                } else if (dfsButton.isSelected()) {
                    startAgent(SearchMode.DFS);
                } else { // bfsButton. isSelected()
                    startAgent(SearchMode.BFS);
                }
                repaint();
            }
        });

        //Agent panel
        JPanel agentSearchPanel = new JPanel();
        agentSearchPanel.setLayout(new GridLayout(4, 1));
        agentSearchPanel.add(aStarButton);
        agentSearchPanel.add(dfsButton);
        agentSearchPanel.add(bfsButton);
        agentSearchPanel.add(startAgentButton);
        agentSearchPanel.setBorder(BorderFactory.createEtchedBorder());

        //info panel
        JPanel infoPanel = new JPanel();
        infoPanel.setLayout(new GridLayout(6, 1));
        infoPanel.add(new JLabel("* black is wall"));
        infoPanel.add(new JLabel("* white is empty space"));
        infoPanel.add(new JLabel("* blue is starting point"));
        infoPanel.add(new JLabel("* red is finishing point"));
        infoPanel.add(new JLabel("* green is visited spaces"));
        infoPanel.add(new JLabel("* orange is user's shortest path"));
        infoPanel.setBorder(BorderFactory.createEtchedBorder());

        //adding everything to each other
        JPanel westPanel = new JPanel();
        westPanel.setLayout(new GridLayout(3, 1));
        westPanel.add(generationPanel);
        westPanel.add(agentSearchPanel);
        westPanel.add(infoPanel);
        add(mazePanel, BorderLayout.CENTER);
        add(westPanel, BorderLayout.WEST);
    }
    
    public void resetState() {
        visitedPos = new LinkedList<>();
        shortestPath = new LinkedList<>();
        visitedPosMap = new boolean[maze.getSizeX()][maze.getSizeY()];
        shortestPathMap = new boolean[maze.getSizeX()][maze.getSizeY()];

        for(int i=0; i<maze.getSizeX(); i++) {
            for(int j=0; j<maze.getSizeY(); j++) {
                visitedPosMap[i][j] = false;
                shortestPathMap[i][j] = false;
            }
        }
    }
            
    public void startAgent(SearchMode mode) {

        SearchAgent agent = null;
        
        switch (mode) {
        case ASTAR:
            agent = new AStarSearchAgent(maze.getSizeX(), maze.getSizeY());
            break;
        case DFS:
            agent = new DepthFirstSearchAgent(maze.getSizeX(), maze.getSizeY());
            break;
        case BFS:
            agent = new BreadthFirstSearchAgent(maze.getSizeX(), maze.getSizeY());
            break;
        }
        
        int x = maze.getStartX();
        int y = maze.getStartY();

        resetState();
        
        while (true) {
            visitedPos.add(new Coordinate(x, y));
            visitedPosMap[x][y] = true;
            
            boolean isExit = (maze.getFinishX() == x && maze.getFinishY() == y);
            
            Coordinate coordinate = null;
            
            switch (mode) {
                case ASTAR:
                    coordinate = ((AStarSearchAgent)agent).move(isExit,
                                                                maze.hasSouthWall(x, y),
                                                                maze.hasNorthWall(x, y),
                                                                maze.hasEastWall(x, y),
                                                                maze.hasWestWall(x, y),
                                                                maze.getEuclideanDistance(x, y));
                    break;
                case DFS:
                    coordinate = ((DepthFirstSearchAgent)agent).move(isExit,
                                                                     maze.hasSouthWall(x, y),
                                                                     maze.hasNorthWall(x, y),
                                                                     maze.hasEastWall(x, y),
                                                                     maze.hasWestWall(x, y));
                    break;
                case BFS:
                    coordinate = ((BreadthFirstSearchAgent)agent).move(isExit,
                                                                       maze.hasSouthWall(x, y),
                                                                       maze.hasNorthWall(x, y),
                                                                       maze.hasEastWall(x, y),
                                                                       maze.hasWestWall(x, y));
                    break;
            }
            
            if (coordinate == null) break; // break the loop
            
            int targetX = maze.getStartX() + coordinate.getX();
            int targetY = maze.getStartY() + coordinate.getY();

            checkValidMove(targetX, targetY, visitedPosMap);
            
            x = targetX;
            y = targetY;
            
            repaint();
        }

        // Get the shortest path from the search algorithms.
        List<Coordinate> path = agent.getShortestPath();
        for(Coordinate a : path) {
            int x2 = maze.getStartX() + a.getX();
            int y2 = maze.getStartY() + a.getY();
            shortestPath.add(new Coordinate(x2, y2));
            shortestPathMap[x2][y2] = true;
        }
    }

    private void checkValidMove(int x, int y, boolean[][] visitedLocation) {
        if (!((0 <= x) && (x < maze.getSizeX()) &&
              (0 <= y) && (y < maze.getSizeY()))) {
            JOptionPane.showMessageDialog(this, "Illegal move. Moving out of the maze");
            System.exit(1);
        }
        
        if (maze.isWall(x, y)) {
            JOptionPane.showMessageDialog(this, "Illegal move. Cannot pass through wall");
            System.exit(1);
        }
            
        if ( ( (x == 0) || !visitedLocation[x-1][y] ) &&
             ( (x == maze.getSizeX()-1) || !visitedLocation[x+1][y] ) &&
             ( (y == 0) || !visitedLocation[x][y-1] ) &&
             ( (y == maze.getSizeY()-1) || !visitedLocation[x][y+1] ) ) {
            JOptionPane.showMessageDialog(this, "Illegal move. Not adjacent to visited locations");
            System.exit(1);
        }
            
    }

}
