
"""
Train a regression model to forecast future flux curves
from past absorption coefficients and link parameters.
"""

import os
import pandas as pd
from sklearn.ensemble import RandomForestRegressor
import joblib

# Parameters
TRAIN_CSV    = "data/training.csv"
MODEL_DIR    = "models"
MODEL_PATH   = os.path.join(MODEL_DIR, "flux_predictor.joblib")

def main():
    # 1. Load training data
    df = pd.read_csv(TRAIN_CSV)
    
    # 2. Identify feature and label columns
    feature_cols = [c for c in df.columns if c.startswith("alpha_")] + \
                   ["lambda_nm", "distance_m"]
    label_cols   = [c for c in df.columns if c.startswith("flux_")]
    
    X = df[feature_cols].values         # shape: (n_samples, T+2)
    y = df[label_cols].values           # shape: (n_samples, M)
    
    # 3. Train the model
    model = RandomForestRegressor(n_estimators=100, random_state=42)
    print("Training model on {} examples...".format(len(df)))
    model.fit(X, y)
    
    # 4. Save the trained model
    os.makedirs(MODEL_DIR, exist_ok=True)
    joblib.dump(model, MODEL_PATH)
    print(f"Model saved to {MODEL_PATH}")

if __name__ == "__main__":
    main()

