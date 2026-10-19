"""
test_returns_report.py
ENHANCEMENT (Milestone Four -- Databases)

Tests for returns_report.py. They run against the in-memory demo database,
so no MySQL server is needed.

Run with:  python -m unittest test_returns_report -v
"""

import argparse
import unittest

import returns_report as rr


class ReturnsReportTests(unittest.TestCase):

    def setUp(self):
        self.conn = rr.build_demo_database()

    def tearDown(self):
        self.conn.close()

    def test_state_totals_match_rma_row_count(self):
        # Every RMA row belongs to exactly one state, so the totals must match.
        states = rr.fetch(self.conn, "demo", "returns_by_state")
        rma_rows = self.conn.execute("SELECT COUNT(*) FROM RMA").fetchone()[0]
        self.assertEqual(int(states["AmountOfReturns"].sum()), rma_rows)

    def test_states_sorted_highest_first(self):
        states = rr.fetch(self.conn, "demo", "returns_by_state")
        counts = states["AmountOfReturns"].tolist()
        self.assertEqual(counts, sorted(counts, reverse=True))

    def test_return_rate_is_between_0_and_100(self):
        skus = rr.fetch(self.conn, "demo", "return_rate_by_sku")
        self.assertTrue((skus["ReturnRatePct"] >= 0).all())
        self.assertTrue((skus["ReturnRatePct"] <= 100).all())

    def test_return_rate_uses_each_products_own_orders(self):
        # Recompute one SKU's rate by hand and compare.
        skus = rr.fetch(self.conn, "demo", "return_rate_by_sku")
        row = skus.iloc[0]
        orders = self.conn.execute(
            "SELECT COUNT(*) FROM Orders WHERE SKU = ?", (row["SKU"],)).fetchone()[0]
        returns = self.conn.execute(
            "SELECT COUNT(*) FROM RMA r JOIN Orders o ON o.OrderID = r.OrderID "
            "WHERE o.SKU = ?", (row["SKU"],)).fetchone()[0]
        self.assertAlmostEqual(row["ReturnRatePct"], round(returns * 100.0 / orders, 2), places=2)

    def test_sql_injection_attempt_is_treated_as_plain_text(self):
        # A hostile "state" value must not be able to run extra SQL.
        evil = "x'; DROP TABLE RMA; --"
        result = rr.fetch(self.conn, "demo", "returns_by_sku_for_state", {"state": evil})
        self.assertTrue(result.empty)  # no state has that name
        still_there = self.conn.execute("SELECT COUNT(*) FROM RMA").fetchone()[0]
        self.assertGreater(still_there, 0)  # table survived

    def test_top_count_rejects_out_of_range_values(self):
        with self.assertRaises(argparse.ArgumentTypeError):
            rr.top_count("0")
        with self.assertRaises(argparse.ArgumentTypeError):
            rr.top_count("51")
        self.assertEqual(rr.top_count("10"), 10)


if __name__ == "__main__":
    unittest.main()
