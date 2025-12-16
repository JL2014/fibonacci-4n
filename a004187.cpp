// Compile : g++ -O2 a004187.cpp -o a004187 `pkg-config --cflags --libs flint`
#include <iostream>
#include <string>
#include <flint/fmpz.h>
#include <flint/arb.h>

using namespace std;

/* précision binaire: 10000 chiffres décimaux ≈ 10000 * log2(10) ≈ 33219 bits + marge de sécurité */
#define PRECISION_BITS 35000

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <n-as-decimal-string>" << endl;
        return 1;
    }

    const slong prec = PRECISION_BITS;
    fmpz_t n;
    fmpz_init(n);
    if (fmpz_set_str(n, argv[1], 10) != 0) {
        cerr << "Error: invalid value for n" << endl;
        return 2;
    }

    arb_t a, t1, t2, x1, x2, r;
    arb_init(a);
    arb_init(t1);
    arb_init(t2);
    arb_init(x1);
    arb_init(x2);
    arb_init(r);

    arb_set_si(a, 45);
    arb_sqrt(a, a, prec);

    /* x1 = (7 + a) / 2 */
    arb_set_si(x1, 7);
    arb_add(x1, x1, a, prec);
    arb_div_si(x1, x1, 2, prec);

    /* x2 = (7 - a) / 2 */
    arb_set_si(x2, 7);
    arb_sub(x2, x2, a, prec);
    arb_div_si(x2, x2, 2, prec);

    /* t1 = x1^n */
    arb_pow_fmpz(t1, x1, n, prec);

    /* t2 = x2^n */
    arb_pow_fmpz(t2, x2, n, prec);

    /* r = (t1 - t2) / a */
    arb_sub(r, t1, t2, prec);
    arb_div(r, r, a, prec);

    /* Affichage haute précision */
    cout << "raw = ";
    arb_printn(r, prec, 0);
    cout << endl;

    /* Arrondi contrôlé */
    arf_t mid;
    arf_init(mid);

    /* mid = milieu exact de r */
    arf_set(mid, arb_midref(r));

    fmpz_t s;
    fmpz_init(s);

    /* arrondi au plus proche */
    arf_get_fmpz(s, mid, ARF_RND_NEAR);

    /* Vérification de proximité */
    arb_t diff, tmp;
    arb_init(diff);
    arb_init(tmp);

    arb_set_fmpz(tmp, s);
    arb_sub(diff, r, tmp, prec);
    arb_abs(diff, diff);

    /* tolérance ≈ 10^{-900} */
    arb_t tol;
    arb_init(tol);
    arb_set_ui(tol, 1);
    arb_mul_2exp_si(tol, tol, -3000);

    if (!arb_lt(diff, tol)) {
        cerr << "Warning: raw value may not be close enough to an integer or precision too low" << endl;
    }

    cout << "a(";
    fmpz_print(n);
    cout << ") = ";
    fmpz_print(s);
    cout << endl;

    arf_clear(mid);
    arb_clear(diff);
    arb_clear(tmp);
    arb_clear(tol);
    fmpz_clear(s);

    flint_cleanup();
    return 0;
}
