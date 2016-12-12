package ca.uwo.eng.se3313.lab4;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import org.joda.time.DateTime;

import java.net.Socket;

import ca.uwo.eng.se3313.lab4.network.INetworkingConnection;
import ca.uwo.eng.se3313.lab4.network.darryl.MyResponseVisitor;
import ca.uwo.eng.se3313.lab4.network.darryl.SocketConnect;
import ca.uwo.eng.se3313.lab4.network.darryl.SocketManager;
import ca.uwo.eng.se3313.lab4.network.request.LoginRequest;
import ca.uwo.eng.se3313.lab4.network.request.MessageRequest;

/**
 * The main application activity. This utilizes multiple fragments to run, {@link LoginFragment} and
 * {@link RoomFragment}.
 *
 */
public class MainActivity extends AppCompatActivity
        implements LoginFragment.OnInteractionListener,
                   RoomFragment.OnInteractionListener {

    /**
     * Defines the marker tag for logging.
     */
    private static final String TAG = MainActivity.class.getSimpleName();

    /**
     * Used to switch between fragments.
     */
    private FragmentManager mFragmentManager;

    /**
     * Fragment referring to the view of the chat room.
     */
    private RoomFragment mRoomFragment;

    /**
     * The connection to the server.
     */
    private INetworkingConnection mConnection;

    // TODO SE3313A
    // Insert any state here:
    private Handler appHandler;
    private boolean loggedIn = false;   // Only true when logged in and socket is connected.
    private String username = "";
    private boolean roomReady = false;

    // Handler codes
    public static final int SocketAWOL      = 9999;
    public static final int DisplayMessage  = 10000;
    public static final int SendMessage     = 10001;
    public static final int DisplayLogin    = 10002;
    public static final int DisplayError    = 10003;
    public static final int ExpectingLogin  = 10004;

    public String getUsername() {
        return username;
    }

    public Handler getAppHandler() {
        return appHandler;
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // ---------- DO NOT TOUCH THIS ----------
        // Initialize the Main Activity to show the Server information
        mFragmentManager = MainActivity.this.getSupportFragmentManager();
        final FragmentTransaction trans = mFragmentManager.beginTransaction();

        trans.replace(R.id.fragment_root, LoginFragment.newInstance(savedInstanceState), LoginFragment.TAG);

        trans.commit();
        // ---------- END NO TOUCH ----------

        // TODO SE3313A
        // Do any state-related work here. Activity#onCreate() is called when the application starts
        // there is no UI shown, yet, thus can not be accessed. All of the UI components are stored
        // in Fragments.

        // Set up main application event loop
        appHandler = new Handler(Looper.getMainLooper()) {
            boolean expectingLogin = false;

            @Override
            /**
             * This method handles the custom messages generated in this application.
             * @param inputMessage The message that is sent.
             */
            public void handleMessage(Message inputMessage) {
                switch (inputMessage.what) {
                    case SendMessage:
                    {
                        // Make message and send it, and display error if it can't be sent.
                        // NOTE: Maybe move this to a separate function as part of OnInteractionListener?
                        MessageRequest msg = new MessageRequest(DateTime.now(), username, (String)inputMessage.obj);
                        mConnection.send(msg, (Throwable e) -> appHandler.sendMessage(Message.obtain(appHandler, DisplayError, e.getMessage())));
                        break;
                    }
                    case ExpectingLogin:
                        // A login is expected, remember detail.
                        expectingLogin = true;
                        break;
                    case SocketAWOL:
                        // Log user off, dismantle remaining socket infrastructure, and return to login.
                        loggedIn = false;
                        roomReady = false;
                        username = "";
                        expectingLogin = false;

                        // Change back to login fragment
                        final FragmentTransaction trans = mFragmentManager.beginTransaction();
                        trans.replace(R.id.fragment_root, LoginFragment.newInstance(savedInstanceState), LoginFragment.TAG);
                        trans.commit();

                        // Close socket stuff
                        try {
                            mConnection.close();
                            mConnection = null;
                        } catch (Exception e) {
                            // TODO: Find something better than ignoring this.
                        }
                        break;
                    case DisplayError: {
                        // Login went bad
                        if (expectingLogin) {
                            expectingLogin = false;
                        }

                        // Display error
                        Context context = getApplicationContext();
                        CharSequence text = (String) inputMessage.obj;
                        int duration = Toast.LENGTH_SHORT;

                        Toast toast = Toast.makeText(context, text, duration);
                        toast.show();
                        break;
                    }
                    case DisplayLogin: {
                        // Login went well or if login success packet comes back before setting the ExpectingLogin message, handle
                        if (expectingLogin || !loggedIn) {
                            loggedIn = true;
                            expectingLogin = false;
                            showRoomFragment();
                        }


                        // If room is ready
                        if (roomReady) {
                            // Show login
                            mRoomFragment.createUserLoginWrapper((DateTime) ((Object[]) inputMessage.obj)[0], (String) ((Object[]) inputMessage.obj)[1]);
                        } else {
                            // Room not ready, stall! HAX.
                            new AsyncTask<Object, Void, Void>() {
                                @Override
                                protected Void doInBackground(Object... params) {
                                    Log.d("LoginAsync", "Sleeping...");
                                    try {
                                        Thread.sleep(500);  // Sleep 500ms and then try again
                                    } catch (InterruptedException e) {
                                        // Ignore error
                                    }
                                    // Resend message.
                                    Log.d("LoginAsync", "Resend.");
                                    appHandler.sendMessage(Message.obtain(appHandler, DisplayLogin, params[0]));
                                    return null;
                                }
                            }.execute(inputMessage.obj);
                        }
                        break;
                    }
                    case DisplayMessage:
                        // If room is ready
                        if (roomReady) {
                            // Display message
                            mRoomFragment.createMessageWrapper((DateTime) ((Object[]) inputMessage.obj)[0], (String) ((Object[]) inputMessage.obj)[1], (String) ((Object[]) inputMessage.obj)[2]);
                        } else {
                            // Room not ready, stall! HAX.
                            new AsyncTask<Object, Void, Void>() {
                                @Override
                                protected Void doInBackground(Object... params) {
                                    Log.d("LoginAsync", "Sleeping...");
                                    try {
                                        Thread.sleep(500);  // Sleep 500ms and then try again
                                    } catch (InterruptedException e) {
                                        // Ignore error
                                    }
                                    // Resend message.
                                    Log.d("LoginAsync", "Resend.");
                                    appHandler.sendMessage(Message.obtain(appHandler, DisplayMessage, params[0]));
                                    return null;
                                }
                            }.execute(inputMessage.obj);
                        }
                        break;
                }
            }
        };
    }


    /** Called when the user requests a login.
     *
     * @param req   The login request object.
     * @param host  The host to login to.
     * @param port  The port on the host to login to.
     */
    @Override
    public void login(LoginRequest req, String host, int port) {
        // Callbacks
        SocketConnect.SocketConnectError errCb = (Throwable e) -> {
            appHandler.sendMessage(Message.obtain(appHandler, DisplayError, e.getMessage()));
        };
        SocketConnect.SocketConnectError lostCb = (Throwable e) -> {
            // Write results
            appHandler.sendMessage(Message.obtain(appHandler, DisplayError, e.getMessage()));;

            // Publish to handler that the socket has gone AWOL.
            appHandler.sendEmptyMessage(SocketAWOL);
        };

        // Open socket.
        mConnection = new SocketManager(new MyResponseVisitor(appHandler), host, port, errCb, lostCb, (Socket sock) -> {
            // If successful, send the login request.
            mConnection.send(
                    req,
                    (Throwable e) -> {
                        // If sending fails, display error and then kill socket.
                        appHandler.sendMessage(Message.obtain(appHandler, DisplayError, e.getMessage()));
                        try {
                            mConnection.close();
                            mConnection = null;
                        } catch (Exception e2) { /* TODO: Am I supposed to do nothing here? */ }
                    },
                    (INetworkingConnection connection, @NonNull LoginRequest message) -> {
                        // If sending succeeds, wait for ResponseThread to publish results.
                        username = req.getSender();

                        // Let Handler know to expect a login. (Race condition covered in Handler)
                        appHandler.sendEmptyMessage(ExpectingLogin);
                    });
        });
    }

    @Override
    public void onRoomReady() {
        // Signal to handler that room is ready.
        roomReady = true;
    }

    // ------ DO NOT MODIFY BELOW ------

    /**
     * Shows a {@link RoomFragment} and hides the {@link LoginFragment}
     */
    private void showRoomFragment() {
        Log.d(TAG, "Loading the room...");

        mRoomFragment = new RoomFragment();

        final FragmentTransaction trans = mFragmentManager.beginTransaction();

        trans.replace(R.id.fragment_root, mRoomFragment, RoomFragment.TAG);

        trans.commit();
    }

}

