/**
 * Author: Tsz-Chiu Au and Olzhas Kaiyrakhmet (Nickname: olzhabay) 
 * Email: chiu@unist.ac.kr and olzhabay.i@gmail.com 
 */

import javax.swing.JFrame;

public class MazeGame{

    public static void main(String args[]) {
        MazeFrame frame = null;
        if (args.length==0) {
            frame = new MazeFrame();
        } else {
            frame = new MazeFrame(args[0]);
        }
        frame.setTitle("Maze");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

}
