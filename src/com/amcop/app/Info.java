package com.amcop.app;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageView;
import android.widget.TextView;

public class Info extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);

		setContentView(R.layout.info);

		TextView AppInfoTitle = (TextView) findViewById(R.id.AppInfoTitletextView);
		ImageView AppIconImage = (ImageView) findViewById(R.id.AppIconimageView);
		TextView AppName = (TextView) findViewById(R.id.AppNametextView);
		TextView AppVer = (TextView) findViewById(R.id.AppVertextView);
		TextView AppInfo = (TextView) findViewById(R.id.AppInfotextView);
		TextView AppInfo2 = (TextView) findViewById(R.id.AppInfotextView2);

		TextView DevelInfoTitle = (TextView) findViewById(R.id.DevelInfoTitletextView);
		ImageView DevelImage = (ImageView) findViewById(R.id.wrimageView);
		TextView DevelName = (TextView) findViewById(R.id.DevelNametextView);
		TextView DevelMail = (TextView) findViewById(R.id.DevelMailtextView);

		AppInfoTitle.setText(getResources().getString(R.string.Info_Title));
		AppIconImage.setImageResource(R.drawable.ic_launcher);
		AppName.setText(getResources().getString(R.string.app_name));
		AppVer.setText(getResources().getString(R.string.Info_AppVer));
		AppInfo.setText(" " + getResources().getString(R.string.Info_AppInfo));
		AppInfo2.setText(" " + getResources().getString(R.string.Info_AppInfo2));

		DevelInfoTitle
				.setText(getResources().getString(R.string.Info_Producer));
		DevelImage.setImageResource(R.drawable.wr);
		DevelName
				.setText(getResources().getString(R.string.Info_Producer_Name));
		DevelMail
				.setText(getResources().getString(R.string.Info_Producer_Mail));

		DevelMail.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				String[] mailto = { getResources().getString(
						R.string.Info_Producer_Mail) };
				Intent sendIntent = new Intent(Intent.ACTION_SEND);
				sendIntent.putExtra(Intent.EXTRA_SUBJECT,
						"[Check My Andorid :: Developer]");
				sendIntent.setType("text/csv");
				sendIntent.putExtra(Intent.EXTRA_EMAIL, mailto);
				startActivity(Intent.createChooser(sendIntent, "Send email"));
			}
		});
		
		AppInfo2.setOnClickListener(new OnClickListener() {

			public void onClick(View v) {
				// TODO Auto-generated method stub
				Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://github.com/venomwine/AMCOP"));
				startActivity(intent);
			}
		});

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		super.onConfigurationChanged(newConfig);
	}
}
