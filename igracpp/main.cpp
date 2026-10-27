
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <utility>

using namespace std;

class player;

class hero {
protected:
    int health_;
    int max_health_;
    int damage_;

public:
    hero(int health, int damage)
        : health_(health), max_health_(health), damage_(damage) {
    }

    virtual ~hero() = default;

    int get_health() const { return health_; }
    int get_max_health() const { return max_health_; }
    int get_damage() const { return damage_; }

    bool IsAlive() const {
        return health_ > 0;
    }

    void damage(int value) {
        health_ -= value;
        if (health_ < 0) health_ = 0;
    }

    void heal(int value) {
        health_ += value;
        if (health_ > max_health_) health_ = max_health_;
    }

    virtual void perform_action(player* self, player* enemy, vector<string>& history) = 0;
    virtual string get_name() const = 0;
};

hero* get_random_living_hero(const vector<hero*>& heroes) {
    vector<hero*> zhivye;

    for (auto h : heroes) {
        if (h->IsAlive()) {
            zhivye.push_back(h);
        }
    }

    if (zhivye.empty()) return nullptr;

    return zhivye[rand() % zhivye.size()];
}

hero* get_random_injured_hero(const vector<hero*>& heroes) {
    vector<hero*> ranenye;

    for (auto h : heroes) {
        if (h->IsAlive() && h->get_health() < h->get_max_health()) {
            ranenye.push_back(h);
        }
    }

    if (ranenye.empty()) return nullptr;

    return ranenye[rand() % ranenye.size()];
}

class player {
    string name_;
    vector<hero*> heroes_;
    queue<hero*> ochered_;

public:
    player(const string& name) : name_(name) {}

    template<typename T, typename... Args>
    void add_hero(Args&&... args) {
        hero* novy = new T(std::forward<Args>(args)...);
        heroes_.push_back(novy);
        ochered_.push(novy);
    }

    const string& get_name() const {
        return name_;
    }

    const vector<hero*>& get_heroes() const {
        return heroes_;
    }

    bool has_alive_heroes() const {
        for (const hero* h : heroes_) {
            if (h->IsAlive()) return true;
        }
        return false;
    }

    hero* get_next_hero() {
        while (!ochered_.empty()) {
            hero* h = ochered_.front();
            ochered_.pop();

            if (h->IsAlive()) {
                ochered_.push(h);
                return h;
            }
        }

        return nullptr;
    }

    void clear_heroes() {
        for (hero* h : heroes_) {
            delete h;
        }

        heroes_.clear();
        ochered_ = queue<hero*>();
    }

    ~player() {
        clear_heroes();
    }
};

class warrior : public hero {
public:
    warrior(int health, int damage) : hero(health, damage) {}

    void perform_action(player* self, player* enemy, vector<string>& history) override;

    string get_name() const override {
        return "Voin";
    }
};

class mage : public hero {
public:
    mage(int health, int damage) : hero(health, damage) {}

    void perform_action(player* self, player* enemy, vector<string>& history) override;

    string get_name() const override {
        return "Mag";
    }
};

class robber : public hero {
public:
    robber(int health, int damage) : hero(health, damage) {}

    void perform_action(player* self, player* enemy, vector<string>& history) override;

    string get_name() const override {
        return "Razboynik";
    }
};

void warrior::perform_action(player* self, player* enemy, vector<string>& history) {
    hero* target = get_random_living_hero(enemy->get_heroes());

    if (target) {
        target->damage(get_damage());

        history.push_back(
            "Voin igroka " + self->get_name() +
            " nanes " + to_string(get_damage()) + " urona."
        );
    }
    else {
        history.push_back("Voin igroka " + self->get_name() + " ne nashel vraga.");
    }
}

void mage::perform_action(player* self, player* enemy, vector<string>& history) {
    hero* target = get_random_injured_hero(self->get_heroes());

    if (target) {
        int lechenie = get_damage();
        target->heal(lechenie);

        history.push_back(
            "Mag igroka " + self->get_name() +
            " vylechil " + target->get_name() +
            " na " + to_string(lechenie) + " HP."
        );
    }
    else {
        history.push_back("Mag igroka " + self->get_name() + " ne nashel ranenykh.");
    }
}

void robber::perform_action(player* self, player* enemy, vector<string>& history) {
    hero* target = get_random_living_hero(enemy->get_heroes());

    if (!target) {
        history.push_back("Razboynik igroka " + self->get_name() + " ne nashel vraga.");
        return;
    }

    if (rand() % 2 == 0) {
        int uron = get_damage() * 2;
        target->damage(uron);

        history.push_back(
            "Razboynik igroka " + self->get_name() +
            " nanes " + to_string(uron) + " urona."
        );
    }
    else {
        damage(get_damage());

        history.push_back(
            "Razboynik igroka " + self->get_name() +
            " ne udalos skrytsya, on poluchil " +
            to_string(get_damage()) + " urona."
        );
    }
}

void print_game_state(const player& p1, const player& p2, const vector<string>& history) {
    system("cls");

    cout << "IgroK 1:\n";

    for (const hero* h : p1.get_heroes()) {
        cout << "  - " << h->get_name()
            << " | HP: " << h->get_health() << "/" << h->get_max_health()
            << " | Uron: " << h->get_damage()
            << (h->IsAlive() ? "" : " [MERTV]")
            << "\n";
    }

    cout << "\n------------------------------\n\n";
    cout << "IgroK 2:\n";

    for (const hero* h : p2.get_heroes()) {
        cout << "  - " << h->get_name()
            << " | HP: " << h->get_health() << "/" << h->get_max_health()
            << " | Uron: " << h->get_damage()
            << (h->IsAlive() ? "" : " [MERTV]")
            << "\n";
    }

    cout << "\n==============================\n";
    cout << "Istoriya:\n";

    for (const string& sobytie : history) {
        cout << "  > " << sobytie << "\n";
    }

    cout << "==============================\n";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    player player_1("1");
    player_1.add_hero<warrior>(10, 3);
    player_1.add_hero<mage>(10, 2);
    player_1.add_hero<robber>(10, 3);

    player player_2("2");
    player_2.add_hero<warrior>(10, 3);
    player_2.add_hero<mage>(10, 2);
    player_2.add_hero<robber>(10, 3);

    vector<string> history;
    history.push_back("Boy nachalsya!");

    while (true) {
        if (player_1.has_alive_heroes() && player_2.has_alive_heroes()) {
            hero* h1 = player_1.get_next_hero();

            if (h1) {
                h1->perform_action(&player_1, &player_2, history);
            }
        }

        if (!player_2.has_alive_heroes()) {
            history.push_back("IgroK 1 pobedil!");
            print_game_state(player_1, player_2, history);
            cout << "\nIgroK 2 proigral!\n";
            break;
        }

        if (player_1.has_alive_heroes() && player_2.has_alive_heroes()) {
            hero* h2 = player_2.get_next_hero();

            if (h2) {
                h2->perform_action(&player_2, &player_1, history);
            }
        }

        if (!player_1.has_alive_heroes()) {
            history.push_back("IgroK 2 pobedil!");
            print_game_state(player_1, player_2, history);
            cout << "\nIgroK 1 proigral!\n";
            break;
        }

        print_game_state(player_1, player_2, history);
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
