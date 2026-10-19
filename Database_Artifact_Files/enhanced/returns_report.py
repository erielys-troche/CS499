"""
returns_report.py
ENHANCEMENT (Milestone Four -- Databases)

Automated RMA (returns) report for the QuantigrationUpdates database.

In the original artifact I ran every analysis query by hand in the MySQL
terminal and screenshotted the output. This script replaces that: it
calls the stored procedures from 01_database_enhancements.sql, analyzes
the results with pandas, makes charts, and writes an HTML report.

Two modes:
  --mode demo   (default) builds a small made-up SQLite database in memory
                so the script runs anywhere without MySQL. Output is
                clearly labeled DEMO DATA.
  --mode mysql  connects to the real MySQL database and calls the stored
                procedures. Needs SQLAlchemy + PyMySQL (see requirements.txt)
                and these environment variables:
                DB_HOST, DB_NAME, DB_USER, DB_PASSWORD (DB_PORT optional).
                Credentials are never written in the code on purpose.

Examples:
  python returns_report.py
  python returns_report.py --mode mysql --top 15
"""

import argparse
import html
import os
import random
import sqlite3
import sys
from datetime import datetime

import pandas as pd
import matplotlib
matplotlib.use("Agg")  # draw charts to files, no window needed
import matplotlib.pyplot as plt

OUTPUT_DIR = "report_output"
NAVY = "#1E2761"

# ---------------------------------------------------------------------
# Queries. In mysql mode the stored procedure is called. In demo mode the
# same SELECT that lives inside the procedure is run against SQLite.
# Anything a user could type (like a state name) is passed as a bound
# parameter and is never glued into the SQL string.
# ---------------------------------------------------------------------
QUERIES = {
    "returns_by_state": (
        "CALL GetReturnsByState()",
        """
        SELECT c.State AS State, COUNT(*) AS AmountOfReturns
        FROM RMA r
        INNER JOIN Orders o       ON o.OrderID = r.OrderID
        INNER JOIN Collaborator c ON c.CustomerID = o.CustomerID
        GROUP BY c.State
        ORDER BY AmountOfReturns DESC
        """,
    ),
    "return_rate_by_sku": (
        "CALL GetReturnRateBySKU()",
        """
        SELECT o.SKU AS SKU,
               MAX(o.Description) AS ProductDescription,
               COUNT(DISTINCT o.OrderID) AS TotalOrders,
               COUNT(r.RMAID) AS TotalReturns,
               ROUND(COUNT(r.RMAID) * 100.0 / COUNT(DISTINCT o.OrderID), 2) AS ReturnRatePct
        FROM Orders o
        LEFT JOIN RMA r ON r.OrderID = o.OrderID
        GROUP BY o.SKU
        ORDER BY ReturnRatePct DESC
        """,
    ),
    "returns_by_reason": (
        "CALL GetReturnsByReason()",
        """
        SELECT COALESCE(Reason, 'Unknown') AS Reason, COUNT(*) AS AmountOfReturns
        FROM RMA
        GROUP BY COALESCE(Reason, 'Unknown')
        ORDER BY AmountOfReturns DESC
        """,
    ),
    "returns_by_sku_for_state": (
        "CALL GetReturnsBySkuForState(:state)",
        """
        SELECT o.SKU AS SKU, COUNT(*) AS AmountOfReturns
        FROM RMA r
        INNER JOIN Orders o       ON o.OrderID = r.OrderID
        INNER JOIN Collaborator c ON c.CustomerID = o.CustomerID
        WHERE c.State = :state
        GROUP BY o.SKU
        ORDER BY AmountOfReturns DESC
        """,
    ),
}


# ---------------------------------------------------------------------
# Connecting
# ---------------------------------------------------------------------
def connect_mysql():
    """Connect to the real MySQL database using environment variables."""
    required = ["DB_HOST", "DB_NAME", "DB_USER", "DB_PASSWORD"]
    missing = [name for name in required if not os.environ.get(name)]
    if missing:
        sys.exit("Missing environment variable(s): " + ", ".join(missing)
                 + "\nSet them first (see README.md), then run again.")
    try:
        from sqlalchemy import create_engine
        from sqlalchemy.engine import URL
    except ImportError:
        sys.exit("mysql mode needs SQLAlchemy and PyMySQL.\n"
                 "Install them with: pip install -r requirements.txt")

    url = URL.create(
        "mysql+pymysql",
        username=os.environ["DB_USER"],
        password=os.environ["DB_PASSWORD"],
        host=os.environ["DB_HOST"],
        port=int(os.environ.get("DB_PORT", "3306")),
        database=os.environ["DB_NAME"],
    )
    return create_engine(url).connect()


def build_demo_database(seed=499):
    """Build a small made-up database with the same tables as the real one."""
    skus = [
        ("BAS-24-1 C", "Basic Switch 10/100/1000 BaseT 24 port", 0.02),
        ("BAS-48-1 C", "Basic Switch 10/100/1000 BaseT 48 port", 0.20),
        ("BAS-08-1 C", "Basic Switch 10/100/1000 BaseT 8 port", 0.10),
        ("ADV-24-10C", "Advanced Switch 10GigE Copper 24 port", 0.10),
        ("ADV-48-10F", "Advanced Switch 10 GigE Copper/Fiber 44 port copper 4 port fiber", 0.10),
        ("ENT-24-10F", "Enterprise Switch 10GigE SFP+ 24 Port", 0.10),
        ("ENT-48-10F", "Enterprise Switch 10GigE SFP+ 48 port", 0.11),
        ("ENT-24-40F", "Enterprise Switch 40GigE SFP+ 24 port", 0.05),
        ("ENT-48-40F", "Enterprise Switch 40GigE SFP+ 48 port", 0.15),
    ]
    states = [
        "Alabama", "Arizona", "Arkansas", "California", "Colorado", "Connecticut",
        "Delaware", "Florida", "Georgia", "Idaho", "Illinois", "Indiana", "Iowa",
        "Kansas", "Kentucky", "Louisiana", "Maine", "Maryland", "Massachusetts",
        "Michigan", "Minnesota", "Mississippi", "Missouri", "Montana", "Nebraska",
        "Nevada", "New Hampshire", "New Jersey", "New Mexico", "New York",
        "North Carolina", "Ohio", "Oklahoma", "Oregon", "Pennsylvania",
        "Rhode Island", "South Carolina", "Tennessee", "Texas", "Utah", "Vermont",
        "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming",
    ]
    reasons = ["Defective", "Wrong item", "Not as described", "No longer needed"]
    statuses = ["Complete", "Complete", "Pending", "In Progress"]
    steps = ["Awaiting customer documentation", "Credit customer account", "Item received"]

    rng = random.Random(seed)
    conn = sqlite3.connect(":memory:")
    conn.executescript("""
        CREATE TABLE Collaborator (CustomerID INTEGER PRIMARY KEY, State TEXT);
        CREATE TABLE Orders (OrderID INTEGER PRIMARY KEY, CustomerID INTEGER,
                             SKU TEXT, Description TEXT);
        CREATE TABLE RMA (RMAID INTEGER PRIMARY KEY, OrderID INTEGER,
                          Step TEXT, Status TEXT, Reason TEXT);
    """)

    for customer_id in range(1, 3001):
        conn.execute("INSERT INTO Collaborator VALUES (?, ?)",
                     (customer_id, rng.choice(states)))

    rma_id = 1
    for order_id in range(1, 6001):
        customer_id = rng.randint(1, 3000)
        sku, description, return_chance = rng.choice(skus)
        conn.execute("INSERT INTO Orders VALUES (?, ?, ?, ?)",
                     (order_id, customer_id, sku, description))
        if rng.random() < return_chance:
            conn.execute("INSERT INTO RMA VALUES (?, ?, ?, ?, ?)",
                         (rma_id, order_id, rng.choice(steps),
                          rng.choice(statuses), rng.choice(reasons)))
            rma_id += 1
    conn.commit()
    return conn


# ---------------------------------------------------------------------
# Running queries
# ---------------------------------------------------------------------
def fetch(connection, mode, name, params=None):
    """Run one named query and return the result as a pandas DataFrame."""
    mysql_sql, demo_sql = QUERIES[name]
    if mode == "mysql":
        from sqlalchemy import text
        result = connection.execute(text(mysql_sql), params or {})
        return pd.DataFrame(result.fetchall(), columns=list(result.keys()))
    return pd.read_sql(demo_sql, connection, params=params)


# ---------------------------------------------------------------------
# Analysis
# ---------------------------------------------------------------------
def summarize_states(states_df):
    """Basic summary statistics for returns by state."""
    counts = states_df["AmountOfReturns"].astype(float)
    return {
        "States with returns": int(len(states_df)),
        "Total returns": int(counts.sum()),
        "Average per state": round(counts.mean(), 1),
        "Median per state": round(counts.median(), 1),
        "Std. deviation": round(counts.std(), 1),
        "Highest": f'{states_df.iloc[0]["State"]} ({int(states_df.iloc[0]["AmountOfReturns"])})',
        "Lowest": f'{states_df.iloc[-1]["State"]} ({int(states_df.iloc[-1]["AmountOfReturns"])})',
    }


def build_findings(states_df, sku_df, reason_df):
    """Turn the numbers into a few plain-English findings."""
    sku_df = sku_df.copy()
    sku_df["ReturnRatePct"] = sku_df["ReturnRatePct"].astype(float)
    worst = sku_df.iloc[0]
    best = sku_df.iloc[-1]
    findings = [
        f'{states_df.iloc[0]["State"]} has the most returns '
        f'({int(states_df.iloc[0]["AmountOfReturns"])}).',
        f'{worst["SKU"]} has the highest return rate at {worst["ReturnRatePct"]}% '
        f'of its orders.',
        f'{best["SKU"]} has the lowest return rate at {best["ReturnRatePct"]}%.',
    ]
    if len(reason_df) > 0:
        findings.append(f'The most common return reason is '
                        f'"{reason_df.iloc[0]["Reason"]}" '
                        f'({int(reason_df.iloc[0]["AmountOfReturns"])} returns).')
    return findings


# ---------------------------------------------------------------------
# Charts
# ---------------------------------------------------------------------
def save_bar_chart(labels, values, title, xlabel, ylabel, filename, horizontal=False):
    fig, ax = plt.subplots(figsize=(9, 5))
    if horizontal:
        ax.barh(labels[::-1], values[::-1], color=NAVY)
        ax.set_xlabel(ylabel)
        ax.set_ylabel(xlabel)
    else:
        ax.bar(labels, values, color=NAVY)
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right")
    ax.set_title(title)
    fig.tight_layout()
    path = os.path.join(OUTPUT_DIR, filename)
    fig.savefig(path, dpi=110)
    plt.close(fig)
    return filename


# ---------------------------------------------------------------------
# HTML report
# ---------------------------------------------------------------------
def write_html_report(mode, summary, findings, tables, charts, top_state, top_state_df):
    banner = ""
    if mode == "demo":
        banner = ('<p class="demo"><strong>DEMO DATA:</strong> this report was built from '
                  'made-up sample data, not the real QuantigrationUpdates database.</p>')

    summary_rows = "".join(
        f"<tr><td>{html.escape(str(k))}</td><td>{html.escape(str(v))}</td></tr>"
        for k, v in summary.items())
    findings_list = "".join(f"<li>{html.escape(f)}</li>" for f in findings)
    chart_tags = "".join(
        f'<h3>{html.escape(title)}</h3><img src="{html.escape(fname)}" alt="{html.escape(title)}">'
        for title, fname in charts)
    table_sections = "".join(
        f"<h3>{html.escape(title)}</h3>{df.to_html(index=False, escape=True)}"
        for title, df in tables)
    state_detail = (f"<h3>Returns by product in {html.escape(str(top_state))}</h3>"
                    + top_state_df.to_html(index=False, escape=True))

    page = f"""<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8">
<title>RMA Returns Report</title>
<style>
 body {{ font-family: Arial, sans-serif; max-width: 900px; margin: 30px auto; color: #222; }}
 h1, h2, h3 {{ color: {NAVY}; }}
 table {{ border-collapse: collapse; margin-bottom: 20px; }}
 th, td {{ border: 1px solid #ccc; padding: 6px 10px; text-align: left; }}
 th {{ background: #e8ecf8; }}
 img {{ max-width: 100%; }}
 .demo {{ background: #fff3cd; padding: 10px; border: 1px solid #e0c36a; }}
</style></head><body>
<h1>RMA Returns Report</h1>
<p>Generated {datetime.now().strftime("%Y-%m-%d %H:%M")}</p>
{banner}
<h2>Key findings</h2><ul>{findings_list}</ul>
<h2>Summary of returns by state</h2><table>{summary_rows}</table>
<h2>Charts</h2>{chart_tags}
<h2>Details</h2>{table_sections}{state_detail}
<h2>Limitations</h2>
<p>This report counts every RMA record as a return. The data does not say whether a
return was caused by the product itself, shipping damage, or customer preference, so
these numbers show where to look, not why returns happen.</p>
</body></html>"""

    path = os.path.join(OUTPUT_DIR, "returns_report.html")
    with open(path, "w", encoding="utf-8") as f:
        f.write(page)
    return path


# ---------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------
def top_count(value):
    """argparse helper: --top must be a whole number from 1 to 50."""
    number = int(value)
    if number < 1 or number > 50:
        raise argparse.ArgumentTypeError("--top must be between 1 and 50")
    return number


def main():
    parser = argparse.ArgumentParser(description="Automated RMA returns report")
    parser.add_argument("--mode", choices=["demo", "mysql"], default="demo")
    parser.add_argument("--top", type=top_count, default=10,
                        help="how many states to show in the chart (1-50)")
    args = parser.parse_args()

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    if args.mode == "demo":
        print("DEMO MODE: using generated sample data, not the real database.")
        connection = build_demo_database()
    else:
        connection = connect_mysql()

    try:
        states_df = fetch(connection, args.mode, "returns_by_state")
        sku_df = fetch(connection, args.mode, "return_rate_by_sku")
        reason_df = fetch(connection, args.mode, "returns_by_reason")

        if states_df.empty or sku_df.empty:
            sys.exit("No return data found. Check that the tables are loaded.")

        top_state = states_df.iloc[0]["State"]
        top_state_df = fetch(connection, args.mode, "returns_by_sku_for_state",
                             {"state": top_state})
    finally:
        connection.close()  # always leave the connection closed

    sku_df["ReturnRatePct"] = sku_df["ReturnRatePct"].astype(float)

    summary = summarize_states(states_df)
    findings = build_findings(states_df, sku_df, reason_df)

    top = states_df.head(args.top)
    charts = [
        ("Top states by number of returns",
         save_bar_chart(top["State"].tolist(), top["AmountOfReturns"].astype(float).tolist(),
                        f"Top {len(top)} states by returns", "State", "Returns",
                        "returns_by_state.png")),
        ("Return rate by product",
         save_bar_chart(sku_df["SKU"].tolist(), sku_df["ReturnRatePct"].tolist(),
                        "Return rate by product (% of that product's orders)",
                        "Product SKU", "Return rate (%)", "return_rate_by_sku.png",
                        horizontal=True)),
    ]
    tables = [
        ("Return rate by product", sku_df),
        ("Returns by reason", reason_df),
    ]

    path = write_html_report(args.mode, summary, findings, tables, charts,
                             top_state, top_state_df)
    print("Report written to", path)
    for finding in findings:
        print(" -", finding)


if __name__ == "__main__":
    main()
