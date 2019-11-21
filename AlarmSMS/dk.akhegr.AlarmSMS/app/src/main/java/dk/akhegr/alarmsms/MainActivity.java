package dk.akhegr.alarmsms;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.net.Uri;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.telephony.SmsManager;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
//import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;

//import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

//import android.R;

public class MainActivity extends Activity {

	//private Button shareIntent;
	private ImageButton connect;
	private ImageButton disconnect;
	private ImageButton status;
	private EditText phoneNo;
	private EditText password;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		phoneNo = (EditText) findViewById(R.id.mobileNumber);
		connect = (ImageButton) findViewById(R.id.connect);
		disconnect = (ImageButton) findViewById(R.id.disconnect);
		status = (ImageButton) findViewById(R.id.status);
		password = (EditText) findViewById(R.id.password);

		connect.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				String number = phoneNo.getText().toString();
				String sms = "#"+password.getText().toString()+getResources().getString(R.string.sms_connect);

				try {
					SmsManager smsManager = SmsManager.getDefault();
					smsManager.sendTextMessage(number, null, sms, null, null);
					Toast.makeText(getApplicationContext(), "SMS er sendt!",
							Toast.LENGTH_LONG).show();
				} catch (Exception e) {
					Toast.makeText(getApplicationContext(),
							"SMS fejlede, forsøg igen senere!",
							Toast.LENGTH_LONG).show();
					e.printStackTrace();
				}
			}
		});

		disconnect.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				String number = phoneNo.getText().toString();
				String sms = "#"+password.getText().toString()+getResources().getString(R.string.sms_disconnect);

				try {
					SmsManager smsManager = SmsManager.getDefault();
					smsManager.sendTextMessage(number, null, sms, null, null);
					Toast.makeText(getApplicationContext(), "SMS er sendt!",
							Toast.LENGTH_LONG).show();
				} catch (Exception e) {
					Toast.makeText(getApplicationContext(),
							"SMS fejlede, forsøg igen senere!",
							Toast.LENGTH_LONG).show();
					e.printStackTrace();
				}
			}
		});

		status.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				String number = phoneNo.getText().toString();
				String sms = "#"+password.getText().toString()+getResources().getString(R.string.sms_status);

				try {
					SmsManager smsManager = SmsManager.getDefault();
					smsManager.sendTextMessage(number, null, sms, null, null);
					Toast.makeText(getApplicationContext(), "SMS er sendt!",
							Toast.LENGTH_LONG).show();
				} catch (Exception e) {
					Toast.makeText(getApplicationContext(),
							"SMS fejlede, forsøg igen senere!",
							Toast.LENGTH_LONG).show();
					e.printStackTrace();
				}
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.menu_exit:
				// Exit functionality goes here
				new AlertDialog.Builder(this).setIcon(android.R.drawable.ic_dialog_alert).setTitle(getResources().getString(R.string.menu_exit))
						.setMessage(getResources().getString(R.string.exit_title))
						.setPositiveButton(getResources().getString(R.string.exit_yes), new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								Intent intent = new Intent(Intent.ACTION_MAIN);
								intent.addCategory(Intent.CATEGORY_HOME);
								intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
								startActivity(intent);
								finish();
							}
						}).setNegativeButton(getResources().getString(R.string.exit_no), null).show();
				return true;
			default:
				return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public void onStart() {
		super.onStart();
	}

	@Override
	public void onBackPressed() {
		new AlertDialog.Builder(this).setIcon(android.R.drawable.ic_dialog_alert).setTitle(getResources().getString(R.string.menu_exit))
				.setMessage(getResources().getString(R.string.exit_title))
				.setPositiveButton(getResources().getString(R.string.exit_yes), new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						Intent intent = new Intent(Intent.ACTION_MAIN);
						intent.addCategory(Intent.CATEGORY_HOME);
						intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
						startActivity(intent);
						finish();
					}
				}).setNegativeButton(getResources().getString(R.string.exit_no), null).show();
	}

	@Override
	public void onStop() {
		super.onStop();
	}
}
