package com.poc.p_couds.models;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import androidx.annotation.Nullable;

public class DbSignInUp extends SQLiteOpenHelper {

    private static final String DB_NAME = "SignInUpDataDb";
    private static final int DB_VERSION = 1;

    // TABLE
    private static final String TABLE_USERS = "TableUsers";
    // COLUMN
    private static final String ARTIFACT_COL = "artifact";
    private static final String EMAIL = "email";
    private static final String PASSWORD = "password";


    public DbSignInUp(@Nullable Context context) {
        super(context, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        String query = "CREATE TABLE " + TABLE_USERS + "("
                + ARTIFACT_COL + " INTEGER PRIMARY KEY AUTOINCREMENT, "
                + EMAIL + " TEXT UNIQUE, "
                + PASSWORD + " TEXT)";
        sqLiteDatabase.execSQL(query);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {
        if (i < 2)
        {
            // Add here the updates adn remove the next 2 lines of code
            sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + TABLE_USERS);
            onCreate(sqLiteDatabase);
        }
    }

    public boolean addNewUser(String email, String password)
    {
        if (checkUser(email)) {
            return false;
        }
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();

        values.put(EMAIL, email);
        values.put(PASSWORD, password);

        db.insertWithOnConflict(TABLE_USERS, null ,values, SQLiteDatabase.CONFLICT_IGNORE);
        db.close();
        return false;
    }

    public boolean checkUser(String email) {
        SQLiteDatabase db = this.getReadableDatabase();
        String query = "SELECT " + EMAIL + " FROM " + TABLE_USERS + " WHERE " +
                EMAIL + " = ? " ;
        Cursor cursor = db.rawQuery(query, new String[] { email });
        boolean exists = cursor.getCount() > 0;
        cursor.close();
        db.close();
        return exists;
    }

    public boolean checkLoginUser(String email, String password) {
        SQLiteDatabase db = this.getReadableDatabase();
        String query = "SELECT " + EMAIL + " FROM " + TABLE_USERS + " WHERE " +
                EMAIL + " = ? AND " + PASSWORD + " = ? " ;
        Cursor cursor = db.rawQuery(query, new String[] { email, password });
        boolean exists = cursor.getCount() > 0;
        cursor.close();
        db.close();
        return exists;
    }
}
