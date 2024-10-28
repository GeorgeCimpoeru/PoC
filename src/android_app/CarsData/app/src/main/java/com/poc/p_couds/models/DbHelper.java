package com.poc.p_couds.models;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import androidx.annotation.Nullable;

import com.poc.p_couds.pojo.UpdateHistory;

import java.sql.SQLData;
import java.util.ArrayList;
import java.util.List;

public class DbHelper extends SQLiteOpenHelper {

    private static final String DB_NAME = "CarsDataDb";
    private static final int DB_VERSION = 1;

    // TABLE
    private static final String TABLE_NAME = "UpdateHistory";
    // COLUMN
    private static final String ARTIFACT_COL = "artifact";
    private static final String STATUS_COL = "status";
    private static final String START_TIME_COL = "start_time";
    private static final String SIZE_COL = "size";


    public DbHelper(@Nullable Context context) {
        super(context, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        String query = "CREATE TABLE " + TABLE_NAME + "("
                + ARTIFACT_COL + " TEXT PRIMARY KEY UNIQUE, "
                + STATUS_COL + " TEXT, "
                + START_TIME_COL + " TEXT, "
                + SIZE_COL + " TEXT)";
        sqLiteDatabase.execSQL(query);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {
        if (i < 2)
        {
            // Add here the updates adn remove the next 2 lines of code
            sqLiteDatabase.execSQL("DROP TABLE IF EXISTS " + TABLE_NAME);
            onCreate(sqLiteDatabase);
        }
    }

    public List<UpdateHistory> getAllUpgrades()
    {
        SQLiteDatabase db = this.getWritableDatabase();
        String query = "SELECT * FROM " + TABLE_NAME + " ORDER BY ROWID DESC;";
        Cursor res = db.rawQuery(query,null);
        List<UpdateHistory> listOfUpgrades = new ArrayList<>();

        if (res.moveToFirst())
        {
            do{
                listOfUpgrades.add(new UpdateHistory(
                        res.getString(0),
                        res.getString(1),
                        res.getString(2),
                        res.getString(3)
                ));
            }while(res.moveToNext());
        }
        return  listOfUpgrades;
    }

    public void addNewUpdate(UpdateHistory update)
    {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();

        values.put(ARTIFACT_COL, update.getArtifact());
        values.put(STATUS_COL, update.getStatus());
        values.put(START_TIME_COL, update.getStartTime());
        values.put(SIZE_COL, update.getSize());

        db.insertWithOnConflict(TABLE_NAME, null ,values, SQLiteDatabase.CONFLICT_IGNORE);
    }
}
