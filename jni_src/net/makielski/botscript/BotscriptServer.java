package net.makielski.botscript;

/**
 * The BotscriptServer object is used to control the underlying<br>
 * botscript-server C++ library using the Java Native Interface (JNI).
 */
public class BotscriptServer {

  static {
    System.loadLibrary("botscript-server");
  }

  /** Service handle. Don't touch! */
  public long service = 0;

  /** Constructor. Initiates the native handle. */
  public BotscriptServer() {
    service = createService();
  }

  /** Start the service (blocking) */
  public native void start();

  /**
   * Stop the service (stops blocking start() call).
   *
   * !WARNING!
   * The service handle will be destroyed after a call to stop()!
   * You'll have to create a new BotscriptServer object.
   * The BotscriptServer is NOT reusable!
   */
  public native void stop();

  /** Instanticate */
  public static native long createService();
}
