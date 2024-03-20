package hw4;

import java.util.Thread;

public class Producer extends Thread {

  private BoundedBuffer buffer;

  public Producer(BoundedBuffer b) {
    buffer = b;
  }

  public void run() {
  }
}
