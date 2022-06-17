#include <indiemotion/context.hpp>

namespace indiemotion
{
    Context::Context()
    {
        _next_rev = std::make_shared<Revision>();
        current = RevisionView({_next_rev});
        pending = RevisionView({_next_rev});
        saved = RevisionView();
    }

    void Context::update(std::string key, std::any value)
    {
        auto ptr = std::make_shared<std::any>(value);
        _next_rev->insert_or_assign(key, std::move(ptr));
    }

    void Context::save()
    {
        if (_next_rev->empty())
        {
            return;
        }
        auto rev = std::move(_next_rev);
        _next_rev = std::make_shared<Revision>();
        if (!_revisions.empty())
        {
            for (auto pair : *_revisions.back())
            {
                rev->insert(pair);
            }
        }
        _revisions.push_back(std::move(rev));
        if (_revisions.size() >= 3)
            _revisions.pop_front();

        pending = RevisionView({_next_rev});
        current = RevisionView({_next_rev, _revisions.back()});
        saved = RevisionView({_revisions.back()});
    }

    Context::RevisionView Context::revision(int rev)
    {
        auto i = _revisions.size();
        if (rev >= i)
        {
            return RevisionView();
        }
        auto r = i - rev - 1;
        return RevisionView({_revisions[r]});
    }

    Context::RevisionView Context::rollback()
    {
        _next_rev = std::make_shared<Revision>();
        if (!_revisions.empty())
        {
            auto rev = _revisions[_revisions.size() - 1];
            _revisions.pop_back();

            _next_rev = std::make_shared<Revision>();
            pending = RevisionView({_next_rev});
            current = RevisionView({_next_rev, _revisions.back()});
            saved = RevisionView({_revisions.back()});

            return RevisionView({rev});
        }
        return RevisionView();
    }

    void Context::clear_pending()
    {
        _next_rev = std::make_shared<Revision>();
        pending = RevisionView({_next_rev});
        current = RevisionView({_next_rev, _revisions.back()});
        saved = RevisionView({_revisions.back()});
    }

    Context::RevisionView::RevisionView(std::vector<std::shared_ptr<Revision>>&& revs): _revs(std::move(revs))
    {

    }

    Context::Value Context::RevisionView::operator[] (std::string key) const {
        if (auto some = get(key))
            return some.value();
        // TODO: Custom Error
        throw std::runtime_error("could not find key");
    }

    std::optional<Context::Value> Context::RevisionView::get(std::string key) const {
        for (auto ptr : _revs)
        {
            auto search = ptr->find(key);
            if (search != ptr->end()) {
                return Value(search->second);
            }
        }
        return {};
    }
}