#!/bin/bash

# String to ZMK Bindings Converter
# Converts plain text strings to ZMK keyboard bindings
# Location: .github/workflows/string_to_zmk.sh

string_to_bindings() {
    local input="$1"
    local output=""
    local i=0
    
    while [ $i -lt ${#input} ]; do
        char="${input:$i:1}"
        
        case "$char" in
            # Lowercase letters
            a) output="$output&kp A " ;;
            b) output="$output&kp B " ;;
            c) output="$output&kp C " ;;
            d) output="$output&kp D " ;;
            e) output="$output&kp E " ;;
            f) output="$output&kp F " ;;
            g) output="$output&kp G " ;;
            h) output="$output&kp H " ;;
            i) output="$output&kp I " ;;
            j) output="$output&kp J " ;;
            k) output="$output&kp K " ;;
            l) output="$output&kp L " ;;
            m) output="$output&kp M " ;;
            n) output="$output&kp N " ;;
            o) output="$output&kp O " ;;
            p) output="$output&kp P " ;;
            q) output="$output&kp Q " ;;
            r) output="$output&kp R " ;;
            s) output="$output&kp S " ;;
            t) output="$output&kp T " ;;
            u) output="$output&kp U " ;;
            v) output="$output&kp V " ;;
            w) output="$output&kp W " ;;
            x) output="$output&kp X " ;;
            y) output="$output&kp Y " ;;
            z) output="$output&kp Z " ;;
            
            # Uppercase letters (with shift)
            A) output="$output&kp LS(A) " ;;
            B) output="$output&kp LS(B) " ;;
            C) output="$output&kp LS(C) " ;;
            D) output="$output&kp LS(D) " ;;
            E) output="$output&kp LS(E) " ;;
            F) output="$output&kp LS(F) " ;;
            G) output="$output&kp LS(G) " ;;
            H) output="$output&kp LS(H) " ;;
            I) output="$output&kp LS(I) " ;;
            J) output="$output&kp LS(J) " ;;
            K) output="$output&kp LS(K) " ;;
            L) output="$output&kp LS(L) " ;;
            M) output="$output&kp LS(M) " ;;
            N) output="$output&kp LS(N) " ;;
            O) output="$output&kp LS(O) " ;;
            P) output="$output&kp LS(P) " ;;
            Q) output="$output&kp LS(Q) " ;;
            R) output="$output&kp LS(R) " ;;
            S) output="$output&kp LS(S) " ;;
            T) output="$output&kp LS(T) " ;;
            U) output="$output&kp LS(U) " ;;
            V) output="$output&kp LS(V) " ;;
            W) output="$output&kp LS(W) " ;;
            X) output="$output&kp LS(X) " ;;
            Y) output="$output&kp LS(Y) " ;;
            Z) output="$output&kp LS(Z) " ;;
            
            # Numbers
            0) output="$output&kp N0 " ;;
            1) output="$output&kp N1 " ;;
            2) output="$output&kp N2 " ;;
            3) output="$output&kp N3 " ;;
            4) output="$output&kp N4 " ;;
            5) output="$output&kp N5 " ;;
            6) output="$output&kp N6 " ;;
            7) output="$output&kp N7 " ;;
            8) output="$output&kp N8 " ;;
            9) output="$output&kp N9 " ;;
            
            # Space
            " ") output="$output&kp SPACE " ;;
            
            # Special characters - Shifted numbers
            "!") output="$output&kp LS(N1) " ;;
            "@") output="$output&kp LS(N2) " ;;
            "#") output="$output&kp LS(N3) " ;;
            "$") output="$output&kp LS(N4) " ;;
            "%") output="$output&kp LS(N5) " ;;
            "^") output="$output&kp LS(N6) " ;;
            "&") output="$output&kp LS(N7) " ;;
            "*") output="$output&kp LS(N8) " ;;
            "(") output="$output&kp LS(N9) " ;;
            ")") output="$output&kp LS(N0) " ;;
            
            # Punctuation - Base
            "-") output="$output&kp MINUS " ;;
            "=") output="$output&kp EQUAL " ;;
            "[") output="$output&kp LBKT " ;;
            "]") output="$output&kp RBKT " ;;
            "\\") output="$output&kp BSLH " ;;
            ";") output="$output&kp SEMI " ;;
            "'") output="$output&kp SQT " ;;
            ",") output="$output&kp COMMA " ;;
            ".") output="$output&kp DOT " ;;
            "/") output="$output&kp FSLH " ;;
            "\`") output="$output&kp GRAVE " ;;
            
            # Punctuation - Shifted
            "_") output="$output&kp LS(MINUS) " ;;
            "+") output="$output&kp LS(EQUAL) " ;;
            "{") output="$output&kp LS(LBKT) " ;;
            "}") output="$output&kp LS(RBKT) " ;;
            "|") output="$output&kp LS(BSLH) " ;;
            ":") output="$output&kp LS(SEMI) " ;;
            "\"") output="$output&kp LS(SQT) " ;;
            "<") output="$output&kp LS(COMMA) " ;;
            ">") output="$output&kp LS(DOT) " ;;
            "?") output="$output&kp LS(FSLH) " ;;
            "~") output="$output&kp LS(GRAVE) " ;;
            
            *)
                echo "Warning: Unsupported character '$char' (ASCII $(printf '%d' "'$char")) - skipping" >&2
                ;;
        esac
        
        i=$((i + 1))
    done
    
    # Trim trailing space and return
    echo "$output" | sed 's/ $//'
}

# Main execution
if [ $# -eq 0 ]; then
    echo "Usage: $0 <string>" >&2
    echo "Example: $0 'Hello World!'" >&2
    exit 1
fi

string_to_bindings "$1"