# QuantigrationUpdates RMA Analytics (Milestone Four - Databases)

Original artifact: DAD 220 Project One and Two (MySQL, run by hand in Codio).
Enhanced version: database improvements + an automated Python report.

## Files
- `01_database_enhancements.sql` - indexes, a view, stored procedures, and a read-only account (MySQL)
- `returns_report.py` - Python report (pandas + matplotlib, SQLAlchemy for MySQL)
- `test_returns_report.py` - unit tests (run against the demo database)
- `sample_demo_output/` - example report made from DEMO DATA (not the real data)

## Run the report in demo mode (no MySQL needed)
    pip install pandas matplotlib
    python returns_report.py
    python -m unittest test_returns_report -v
Open `report_output/returns_report.html` in a browser.

## Run it against the real database
1. In MySQL, load the original tables, then run: `SOURCE 01_database_enhancements.sql;`
   (change the placeholder password in that file first)
2. Install the extra libraries: `pip install -r requirements.txt`
3. Set the connection info as environment variables (never type a password into the code):

       export DB_HOST=localhost
       export DB_NAME=QuantigrationUpdates
       export DB_USER=rma_analyst
       export DB_PASSWORD=your_password_here

4. `python returns_report.py --mode mysql --top 15`
