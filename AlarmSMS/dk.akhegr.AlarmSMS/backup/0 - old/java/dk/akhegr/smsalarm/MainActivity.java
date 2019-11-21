package dk.akhegr.alarmsms;

import android.net.Uri;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.telephony.SmsManager;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import dk.akhegr.alarmsms.R;

public class MainActivity extends Activity {

	private Button shareIntent;
	private Button send;
	private EditText phoneNo;
	private EditText messageBody;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		phoneNo = (EditText) findViewById(R.id.mobileNumber);
		messageBody = (EditText) findViewById(R.id.smsBody);

		send = (Button) findViewById(R.id.send);
		send.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				String number = phoneNo.getText().toString();
				String sms = messageBody.getText().toString();

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
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
