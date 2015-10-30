package theo.dexter.ui.fragment;


import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;

import butterknife.Bind;
import butterknife.ButterKnife;
import theo.dexter.R;
import theo.dexter.DexterApplication;
import theo.dexter.bluetooth.BluetoothService;
import theo.dexter.ui.activity.PlayActivity;

/**
 * Fragment to choose from paired bluetooth devices
 */
public class BluetoothFragment extends BaseFragment implements BluetoothService.OnBluetoothDeviceDiscoveredListener, AdapterView.OnItemClickListener{

    private static final int REQUEST_ENABLE_BT = 1;

    @Bind(R.id.device_list)
    ListView deviceList;

    private BluetoothService btService;
    private boolean btBound;

    private ArrayAdapter<String> adapter;
    private ArrayList<BluetoothDevice> devices;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        ((DexterApplication) getActivity().getApplication()).getComponent().inject(this);
        ButterKnife.bind(this, view);

        //Turn on Bluetooth
        if (!BluetoothAdapter.getDefaultAdapter().isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        BluetoothService.bindToService(getActivity(), btConnection);
    }

    @Override
    public void onPause() {
        super.onPause();
        if(btBound){
            btService.stopScan();
            getActivity().unbindService(btConnection);
        }
    }

    /**
     * Clears all data and begins new scan
     */
    private void scan(){
        btService.stopScan();

        devices = new ArrayList<>();
        adapter = new ArrayAdapter<>(getContext(), android.R.layout.simple_list_item_1, new ArrayList<String>());
        deviceList.setAdapter(adapter);
        deviceList.setOnItemClickListener(this);

        btService.startScan(this);
    }


    @Override
    public void OnBluetoothDeviceDiscovered(BluetoothDevice device) {
        devices.add(device);
        adapter.add(device.getName() + "\n" + device.getAddress());
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        BluetoothDevice selected = devices.get(position);

        Toast.makeText(getContext(), selected.getAddress(), Toast.LENGTH_SHORT).show();

        if(btService.connect(selected.getAddress())){
            Intent i = new Intent(getActivity(), PlayActivity.class);
            i.putExtra(PlayActivity.EXTRA_ADDRESS, selected.getAddress());
            startActivity(i);
        }


    }

    @Override
    public int getTitleResourceId() {
        return R.string.fragment_bluetooth_title;
    }

    /** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection btConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                                       IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            BluetoothService.BluetoothServiceBinder binder = (BluetoothService.BluetoothServiceBinder) service;
            btService = binder.getService();
            btBound = true;
            scan();
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            btBound = false;
        }
    };
}
