package ca.uwo.eng.se3313.lab4;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.google.common.base.Function;

import org.joda.time.DateTime;

import ca.uwo.eng.se3313.lab4.network.INetworkingConnection;
import ca.uwo.eng.se3313.lab4.network.request.MessageRequest;
import ca.uwo.eng.se3313.lab4.network.response.LoginResponse;
import ca.uwo.eng.se3313.lab4.network.response.MessageResponse;
import ca.uwo.eng.se3313.lab4.network.response.ResponseVisitor;

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

